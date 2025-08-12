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

#include "Game.h"
#include "Offsets.h"

#include <windows.h>

namespace Game {

namespace Lua {
lua_pushnil_t PushNil = reinterpret_cast<lua_pushnil_t>(Offsets::LUA_PUSH_NIL);
lua_isnumber_t IsNumber = reinterpret_cast<lua_isnumber_t>(Offsets::LUA_IS_NUMBER);
lua_tonumber_t ToNumber = reinterpret_cast<lua_tonumber_t>(Offsets::LUA_TO_NUMBER);
lua_pushnumber_t PushNumber = reinterpret_cast<lua_pushnumber_t>(Offsets::LUA_PUSH_NUMBER);
lua_isstring_t IsString = reinterpret_cast<lua_isstring_t>(Offsets::LUA_IS_STRING);
lua_tostring_t ToString = reinterpret_cast<lua_tostring_t>(Offsets::LUA_TO_STRING);
lua_pushstring_t PushString = reinterpret_cast<lua_pushstring_t>(Offsets::LUA_PUSH_STRING);
lua_error_t Error = reinterpret_cast<lua_error_t>(Offsets::LUA_ERROR);
} // namespace Lua

GetGUIDFromName_t GetGUIDFromName = (GetGUIDFromName_t)Offsets::FUN_GET_GUID_FROM_NAME;
ClntObjMgrObjectPtr_t ClntObjMgrObjectPtr =
    (ClntObjMgrObjectPtr_t)Offsets::FUN_CLNT_OBJ_MGR_OBJECT_PTR;
RenderObjectBlips_t RenderObjectBlips =
    reinterpret_cast<RenderObjectBlips_t>(Offsets::FUN_RENDER_OBJECT_BLIP);
TextureGetGxTex_t TextureGetGxTex =
    reinterpret_cast<TextureGetGxTex_t>(Offsets::FUN_TEXTURE_GET_GX_TEX);
GxRsSet_t GxRsSet = reinterpret_cast<GxRsSet_t>(Offsets::FUN_GX_RS_SET);
GxPrimLockVertexPtrs_t GxPrimLockVertexPtrs =
    reinterpret_cast<GxPrimLockVertexPtrs_t>(Offsets::FUN_GX_PRIM_LOCK_VERTEX_PTRS);
GxPrimDrawElements_t GxPrimDrawElements =
    reinterpret_cast<GxPrimDrawElements_t>(Offsets::FUN_GX_PRIM_DRAW_ELEMENTS);
GxPrimUnlockVertexPtrs_t GxPrimUnlockVertexPtrs =
    reinterpret_cast<GxPrimUnlockVertexPtrs_t>(Offsets::FUN_GX_PRIM_UNLOCK_VERTEX_PTRS);
TextureCreate_t TextureCreate = reinterpret_cast<TextureCreate_t>(Offsets::FUN_TEXTURE_CREATE);
WorldPosToMinimapFrameCoords_t WorldPosToMinimapFrameCoords =
    reinterpret_cast<WorldPosToMinimapFrameCoords_t>(
        Offsets::FUN_WORLD_POS_TO_MINIMAP_FRAME_COORDS);

CStatus::CStatus() {
    vftable = reinterpret_cast<void *>(Offsets::VFTABLE_CSTATUS);
    unk = 8;
    head.prevLink = &head;
    head.next = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(&head) | 1u);
    maxSeverity = STATUS_TYPE::STATUS_INFO;
}

CStatus::~CStatus() {
    reinterpret_cast<CStatus_Destructor_t>(Offsets::FUN_CSTATUS_DESTRUCTOR)(this);
}

CGxTexFlags::CGxTexFlags(EGxTexFilter filter, uint32_t wrapU, uint32_t wrapV,
                         uint32_t forceMipTracking, uint32_t generateMipMaps, uint32_t renderTarget,
                         uint32_t maxAnisotropy, uint32_t unknownFlag) {
    reinterpret_cast<CGxTexFlags_Constructor_t>(Offsets::FUN_CGX_TEX_FLAGS_CONSTRUCTOR)(
        this, filter, wrapU, wrapV, forceMipTracking, generateMipMaps, renderTarget, maxAnisotropy,
        unknownFlag);
}

C3Vector *s_blipVertices = reinterpret_cast<C3Vector *>(Offsets::CONST_BLIP_VERTICES);
TexCoord &texCoords = *reinterpret_cast<TexCoord *>(Offsets::CONST_TEX_COORDS);
C3Vector &normal = *reinterpret_cast<C3Vector *>(Offsets::CONST_NORMAL_VEC3);
unsigned short *vertIndices = reinterpret_cast<unsigned short *>(Offsets::CONST_VERT_INDICES);

void RegisterLuaFunction(const char *name, int(__cdecl *function)(void *L), uintptr_t caveAddress) {
    constexpr uint8_t trampoline[] = {
        0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, 0x00000000
        0xFF, 0xE0                    // jmp eax
    };

    DWORD oldProtect;
    uint8_t patchedTrampoline[sizeof(trampoline)];
    memcpy(patchedTrampoline, trampoline, sizeof(trampoline));

    const auto funcAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(function));
    memcpy(&patchedTrampoline[1], &funcAddr, sizeof(funcAddr));

    VirtualProtect(reinterpret_cast<LPVOID>(caveAddress), sizeof(patchedTrampoline),
                   PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(reinterpret_cast<void *>(caveAddress), patchedTrampoline, sizeof(patchedTrampoline));
    VirtualProtect(reinterpret_cast<LPVOID>(caveAddress), sizeof(patchedTrampoline), oldProtect,
                   &oldProtect);

    auto registerFunction =
        reinterpret_cast<FrameScript_RegisterFunction>(Offsets::FUN_REGISTER_LUA_FUNCTION);
    registerFunction(name, caveAddress);
}

void DrawMinimapTexture(HTEXTURE__ *texture, C2Vector minimapPosition, float scale) {
    const CImVector color = {0xFF, 0xFF, 0xFF, 0xFF}; // White
    // TODO: Add graying blip using isInDifferentArea
    // CImVector color = {0xFF, 0xB0, 0xB0, 0xB0}; //  Dark Gray

    C3Vector vertices[4];
    for (auto i = 0; i < 4; i++) {
        vertices[i].x = minimapPosition.x + scale * s_blipVertices[i].x;
        vertices[i].y = minimapPosition.y + scale * s_blipVertices[i].y;
        vertices[i].z = scale * s_blipVertices[i].z;
    }

    Game::CStatus status;
    CGxTex *gxTex = TextureGetGxTex(texture, 1, &status);
    if (!status.ok())
        return;

    GxRsSet(GxRs_Texture0, gxTex);

    GxPrimLockVertexPtrs(4, vertices, 12, &normal, 0, &color, 0, nullptr, 0,
                         reinterpret_cast<C2Vector *>(&texCoords), 8, nullptr, 0);

    GxPrimDrawElements(EGxPrim::GxPrim_TriangleStrip, 4, vertIndices);

    GxPrimUnlockVertexPtrs();
}

} // namespace Game
