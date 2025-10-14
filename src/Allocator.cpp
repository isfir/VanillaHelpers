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

#include "Allocator.h"
#include "Common.h"
#include "Offsets.h"

#include <stdint.h>

namespace Allocator {

static void PatchBiggerMemoryRegion() {
    // 32 MiB regions (128 regions -> 4 GiB virtual address space)
    const uint8_t exponent = 0x19;                            // 25 -> 2^25 = 32 MiB
    const uint8_t shift = static_cast<uint8_t>(exponent - 1); // 24
    const uint32_t size = 1U << exponent;                     // 0x02000000
    const uint32_t sizeNeg = 0U - size;                       // 0xFE000000

    // Reservatio
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_RESERVE_PUSH_SIZE + 1), &size,
                       sizeof(size)); // push imm32

    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_RESERVE_ACCOUNT_INC + 3), &size,
                       sizeof(size)); // add [eax+14], imm32

    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_RESERVE_ACCOUNT_DEC + 3),
                       &sizeNeg, sizeof(sizeNeg)); // add [eax+14], imm32 (negative)

    // Validator
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_VALIDATE_REGION_END_LEA + 2),
                       &size, sizeof(size)); // lea eax,[esi+imm32]

    // Small vs large
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_SMALL_LARGE_CMP_ALLOC_A + 2),
                       &exponent, sizeof(exponent)); // cmp ebx, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_SMALL_LARGE_CMP_ALLOC_B + 2),
                       &exponent, sizeof(exponent)); // cmp ebx, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_SMALL_LARGE_CMP_FREE_A + 2),
                       &exponent, sizeof(exponent)); // cmp edi, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_SMALL_LARGE_CMP_FREE_B + 2),
                       &exponent, sizeof(exponent)); // cmp edi, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_SMALL_LARGE_CMP_FREE_C + 2),
                       &exponent, sizeof(exponent)); // cmp edi, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_SMALL_LARGE_CMP_FREE_D + 2),
                       &exponent, sizeof(exponent)); // cmp edi, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_SMALL_LARGE_CMP_ISVALID + 2),
                       &exponent, sizeof(exponent)); // cmp ecx, imm8

    // Bitfield packing/unpacking
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_PACK_SHR_ENCODE + 2), &shift,
                       sizeof(shift)); // shr ecx, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_PACK_SHL_ENCODE + 2), &shift,
                       sizeof(shift)); // shl eax, imm8
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_SMEM_PACK_SHL_DECODE + 2), &shift,
                       sizeof(shift)); // shl edx, imm8
}

void Initialize() { PatchBiggerMemoryRegion(); }

} // namespace Allocator
