// This file is part of VanillaHelpers.
//
// VanillaHelpers is free software: you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software Foundation, either version 3
// of the License, or (at your option) any later version.
//
// VanillaHelpers is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lessed General Public License along with
// VanillaHelpers. If not, see <https://www.gnu.org/licenses/>.

#include "Texture.h"
#include "Common.h"
#include "Game.h"
#include "MinHook.h"
#include "Offsets.h"

#include <cstdint>
#include <windows.h>

namespace Texture {

static Game::TextureBlit_t TextureBlitBlend_o = nullptr;
static Game::TextureBlit_t TextureBlitMasked_o = nullptr;
static Game::TextureBlit_t TextureBlitCopy_o = nullptr;

// 32..1024, 6x6 = 36 buckets
static constexpr uint32_t BUCKETS_PER_SIDE = 6;
static constexpr uint32_t BUCKET_COUNT = BUCKETS_PER_SIDE * BUCKETS_PER_SIDE;
static constexpr uint32_t BUCKET_STRIDE = 12; // [param][head][prev]

static void *g_asyncJmpUnlink = nullptr;
static void *g_asyncJmpAlready = nullptr;
static void *g_asyncBuf = nullptr;

static uint8_t *g_bucketBlock = nullptr; // contiguous block for [param][head][prev] arrays
static uint32_t g_baramBase = 0;
static uint32_t g_headBase = 0;
static uint32_t g_prevBase = 0;

static __declspec(naked) void AsyncTextureWait_Atomic_h() {
    __asm {
        // Save req pointer: req = [EDI + 0x138]
        mov     ecx, [edi + 0x138] // ECX = req

            // Atomic test-and-set on a per-object guard byte at [this + 0x155]
        xor     eax, eax // AL = 0 (expected)
        mov     dl, 1 // DL = 1 (new)
        lock cmpxchg byte ptr [edi + 0x155], dl

            // Pop the two pushes performed just before 0x44AD6F
        add     esp, 8

        // ZF=1 => we transitioned 0->1: go do unlink (0x44AD7F)
        jz      first

            // ZF=0 => someone else already set it: original JNZ target (0x44ADEB)
        jmp     dword ptr [g_asyncJmpAlready]

first:
        // 0x44AD7F expects EAX=req
        mov     eax, ecx
        jmp     dword ptr [g_asyncJmpUnlink]
    }
}

static void InstallNewAsyncFileBuffer() {
    const uint32_t newSize = 0x2000000; // 32 MiB
    g_asyncBuf = VirtualAlloc(nullptr, newSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!g_asyncBuf)
        return;

    const auto newBase = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_asyncBuf));

    // Patch buffer address (LEA ..., [reg + imm32])
    Common::PatchBytes(
        reinterpret_cast<void *>(Offsets::PATCH_ASYNC_BUFFER_ADDR_TEX_LOAD_IMAGE + 2), &newBase,
        sizeof(newBase)); // TextureLoadImage
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_ASYNC_BUFFER_ADDR_BLP_PATH + 2),
                       &newBase, sizeof(newBase)); // IModelCreate/BLP path
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_ASYNC_BUFFER_ADDR_PUMP + 2),
                       &newBase, sizeof(newBase)); // Pump/refill

    // Patch buffer max size (MOV r32, imm32)
    Common::PatchBytes(
        reinterpret_cast<void *>(Offsets::PATCH_ASYNC_BUFFER_SIZE_TEX_LOAD_IMAGE + 1), &newSize,
        sizeof(newSize)); // TextureLoadImage
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_ASYNC_BUFFER_SIZE_BLP_PATH + 1),
                       &newSize, sizeof(newSize)); // IModelCreate/BLP path
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_ASYNC_BUFFER_SIZE_PUMP + 1),
                       &newSize, sizeof(newSize)); // Pump/refill

    // Reset usage counter so the very first scheduling starts at newBase+0
    const uint32_t zero = 0;
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::VAR_ASYNC_BUFFER_USAGE_COUNTER), &zero,
                       sizeof(zero));
}

static bool InitializeNewTexturePool() {
    const uint32_t stride = BUCKET_STRIDE; // 12
    const uint32_t count = BUCKET_COUNT;   // 36
    const size_t bucketBytes = static_cast<size_t>(count) * stride;

    g_bucketBlock = static_cast<uint8_t *>(
        VirtualAlloc(nullptr, bucketBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!g_bucketBlock)
        return false;

    // Lay out param/head/prev with the expected relative positions:
    g_baramBase = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_bucketBlock));
    g_headBase = g_baramBase + 4;
    g_prevBase = g_baramBase + 8;

    // Initialize buckets
    // [param] = 8, [head] = self, [prev] = (head|1).
    for (uint32_t i = 0; i < count; ++i) {
        uint8_t *p = g_bucketBlock + i * stride;
        const uint32_t headAdr = g_headBase + i * stride;
        const uint32_t prevAdr = headAdr | 1;
        const uint32_t paramVal = 8;

        memcpy(p + 0, &paramVal, sizeof(paramVal));
        memcpy(p + 4, &headAdr, sizeof(headAdr));
        memcpy(p + 8, &prevAdr, sizeof(prevAdr));
    }

    return true;
}

static void PatchNewTexturePool() {
    const uint32_t side32 = BUCKETS_PER_SIDE;                       // 6
    const uint8_t side8 = static_cast<uint8_t>(side32);             // 6
    const uint32_t count = BUCKET_COUNT;                            // 36
    const uint32_t stride = BUCKET_STRIDE;                          // 12
    const uint8_t rowStep8 = static_cast<uint8_t>(side32 * stride); // 6*12 = 72 (0x48)
    const uint32_t totalBytes = count * stride;                     // 36*12 = 432 (0x1B0)
    const uint32_t headEnd = g_headBase + totalBytes;               // one past last head

    // Pool initializer
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_INIT_POOL_HEAD + 1),
                       &g_headBase, sizeof(g_headBase)); // MOV EAX, imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_INIT_POOL_COUNT + 1), &count,
                       sizeof(count)); // MOV ECX, imm32

    // Cleaner (iterate buckets backwards)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_CLEANUP_START_EDI + 1),
                       &headEnd, sizeof(headEnd)); // MOV EDI, imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_CLEANUP_COUNT + 3), &count,
                       sizeof(count)); // MOV [local_8], imm32

    // TextureAllocGxTex: prev/head bases
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ALLOC_PREV + 3), &g_prevBase,
                       sizeof(g_prevBase)); // 8B 1C 8D imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ALLOC_HEAD + 3), &g_headBase,
                       sizeof(g_headBase)); // 8D 04 8D imm32

    // TextureFreeGxTex: param base (two sites)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_FREE_PARAM_1 + 3),
                       &g_baramBase, sizeof(g_baramBase)); // 8B 34 95 imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_FREE_PARAM_2 + 3),
                       &g_baramBase, sizeof(g_baramBase)); // 8D 1C 95 imm32

    // Iterators (0x448EC0): start@prev, outer=side, inner=side
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ITERA_PREV_LOAD + 1),
                       &g_prevBase, sizeof(g_prevBase)); // MOV EAX, imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ITERA_OUTER_CNT + 3),
                       &side32, sizeof(side32)); // MOV [local_10], imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ITERA_INNER_CNT + 3),
                       &side32, sizeof(side32)); // MOV [local_c], imm32

    // Iterators (0x449010): start@prev, outer=side, inner cmp=side, row step = side*stride
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ITERB_PREV_LOAD + 1),
                       &g_prevBase, sizeof(g_prevBase)); // MOV EDI, imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ITERB_OUTER_CNT + 3),
                       &side32, sizeof(side32)); // MOV [local_10], imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ITERB_INNER_CMP + 2), &side8,
                       sizeof(side8)); // 83 F8 imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_ITERB_ROW_STEP + 2),
                       &rowStep8, sizeof(rowStep8)); // 83 C7 imm8

    // Stats/diagnostics (0x44B4F0): prev/param bases; outer=side; total range = count*stride
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_STAT_PREV_BASE + 2),
                       &g_prevBase, sizeof(g_prevBase)); // 8B 9E imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_STAT_PARAM_BASE + 2),
                       &g_baramBase, sizeof(g_baramBase)); // 8B 86 imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_STAT_OUTER_CNT + 3), &side32,
                       sizeof(side32)); // MOV [local_10+4], imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXPOOL_STAT_TOTAL_END + 2),
                       &totalBytes, sizeof(totalBytes)); // 81 FE imm32
}

static void InstallNewTexturePool() {
    if (InitializeNewTexturePool())
        PatchNewTexturePool();
}

static void PatchBiggerCharacterSkin(uint8_t shift) {
    const uint32_t newSize = 0x100U << shift;
    const uint32_t newStride = newSize * 2;
    const uint32_t fullClear = newSize - 1;

    // Atlas coordinates
    DWORD oldProt;
    auto *coordinate =
        reinterpret_cast<uint32_t *>(Offsets::CONST_CHARACTER_SKIN_ATLAS_COORDINATES);
    VirtualProtect(coordinate, 0xA0, PAGE_READWRITE, &oldProt);
    for (int i = 0; i < 40; ++i)
        coordinate[i] <<= shift;
    VirtualProtect(coordinate, 0xA0, oldProt, &oldProt);

    // Gating and TextureCreate parameters
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_GATE_DIM_CMP + 1),
                       &newSize, sizeof(newSize)); // CMP EAX, imm32
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_CREATE_SIZE_PUSH + 1),
                       &newSize, sizeof(newSize)); // PUSH imm32 (width)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_CREATE_SIZE_MOV_EDX + 1),
                       &newSize, sizeof(newSize)); // MOV EDX, imm32 (height)

    // CPU compositor stride (three blitters: copy/masked/blend)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_COMPOS_COPY_STRIDE + 3),
                       &newStride, sizeof(newStride)); // MOV dword ptr [local_c], imm32

    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_COMPOS_MASKED_STRIDE + 3),
                       &newStride, sizeof(newStride)); // MOV dword ptr [local_c], imm32

    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_COMPOS_BLEND_STRIDE + 3),
                       &newStride, sizeof(newStride)); // MOV dword ptr [local_c], imm32

    // Clear (not sure if needed)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_CLEAR_COLOR_R + 1),
                       &fullClear, sizeof(fullClear));
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_CLEAR_COLOR_G + 1),
                       &fullClear, sizeof(fullClear));

    // Character Component init (not sure if needed)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_TABLE_SIDE_PUSH + 1),
                       &newSize, sizeof(newSize));
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_TABLE_SIDE_MOV_EDX + 1),
                       &newSize, sizeof(newSize));

    // Engine level clamps (not sure if needed)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEX_DIM_CLAMP_W + 2), &newSize,
                       sizeof(newSize));
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEX_DIM_CLAMP_H + 2), &newSize,
                       sizeof(newSize));

    // Cleanup constants (not sure if needed)
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_FREE_SIDE_PUSH_W + 1),
                       &newSize, sizeof(newSize));
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_CHAR_ATLAS_FREE_SIDE_PUSH_H + 1),
                       &newSize, sizeof(newSize));
}

static uint8_t GetCharacterSkinSizeSetting() {
    uint8_t size = 0;
    Game::SFile *file = nullptr;

    if (!Game::SFile_Open(R"(VanillaHelpers\ResizeCharacterSkin.txt)", &file) || file == nullptr) {
        return size;
    }

    if (Game::SFile_Read(file, &size, sizeof(size), nullptr, nullptr, nullptr)) {
        if (isdigit(size)) {
            size = size - '0';
        }
    }

    Game::SFile_Close(file);
    return size;
}

// It will return true (it shouldn't) when textures are too big for the character skin, this isn't
// desirable, but this was the best I could do with my knowledge of those functions.
static inline bool TextureBlitFits(const Game::Point *offset, const Game::Rectangle *copy,
                                   const Game::Rectangle *extent) {
    if (offset->x >= extent->width || offset->y >= extent->heigth)
        return false;
    const uint32_t remainW = extent->width - offset->x;
    const uint32_t remainH = extent->heigth - offset->y;
    return copy->width <= remainW && copy->heigth <= remainH;
}

static void __stdcall TextureBlitCopy_h(void *unk0, void *unk1, Game::Point *srcOffset,
                                        Game::Point *dstOffset, Game::Rectangle *copy,
                                        Game::Rectangle *dstExtent) {
    if (!TextureBlitFits(dstOffset, copy, dstExtent)) {
        return;
    }
    TextureBlitCopy_o(unk0, unk1, srcOffset, dstOffset, copy, dstExtent);
}

static void __stdcall TextureBlitMasked_h(void *unk0, void *unk1, Game::Point *dstOffset,
                                          Game::Point *srcOffset, Game::Rectangle *copy,
                                          Game::Rectangle *srcExtent) {
    if (!TextureBlitFits(srcOffset, copy, srcExtent)) {
        return;
    }

    TextureBlitMasked_o(unk0, unk1, dstOffset, srcOffset, copy, srcExtent);
}

static void __stdcall TextureBlitBlend_h(void *unk0, void *unk1, Game::Point *dstOffset,
                                         Game::Point *srcOffset, Game::Rectangle *copy,
                                         Game::Rectangle *srcExtent) {
    if (!TextureBlitFits(srcOffset, copy, srcExtent)) {
        return;
    }

    TextureBlitBlend_o(unk0, unk1, dstOffset, srcOffset, copy, srcExtent);
}

void Initialize() {
    // Enable 1024-sized textures
    const uint8_t ja = 0x87;
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXTURE_SIZE_1 + 1), &ja,
                       sizeof(ja));
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXTURE_SIZE_2 + 1), &ja,
                       sizeof(ja));

    // Currently crashes with a buffer bigger than 2 MiB while loading textures bigger than 2 MiB
    InstallNewAsyncFileBuffer();

    InstallNewTexturePool();
}

bool InstallHooks() {
    // Fix a bug with multiple threads trying to unlink
    g_asyncJmpUnlink = reinterpret_cast<void *>(Offsets::PATCH_ASYNC_TEXTURE_WAIT_ATOMIC_UNLINK);
    g_asyncJmpAlready = reinterpret_cast<void *>(Offsets::PATCH_ASYNC_TEXTURE_WAIT_ATOMIC_ALREADY);

    auto *target = reinterpret_cast<LPVOID>(Offsets::PATCH_ASYNC_TEXTURE_WAIT_ATOMIC);
    if (MH_CreateHook(target, static_cast<LPVOID>(AsyncTextureWait_Atomic_h), nullptr) != MH_OK)
        return FALSE;
    if (MH_EnableHook(target) != MH_OK)
        return FALSE;

    HOOK_FUNCTION(Offsets::FUN_TEXTURE_BLIT_COPY, TextureBlitCopy_h, TextureBlitCopy_o);
    HOOK_FUNCTION(Offsets::FUN_TEXTURE_BLIT_MASKED, TextureBlitMasked_h, TextureBlitMasked_o);
    HOOK_FUNCTION(Offsets::FUN_TEXTURE_BLIT_BLEND, TextureBlitBlend_h, TextureBlitBlend_o);

    return TRUE;
}

void InstallCharacterSkin() {
    uint8_t size = GetCharacterSkinSizeSetting();
    switch (size) {
    case 2:
        PatchBiggerCharacterSkin(1);
        break;
    case 4:
        PatchBiggerCharacterSkin(2);
        break;
    default:
        break;
    }
}

} // namespace Texture
