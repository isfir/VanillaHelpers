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
#include "Offsets.h"

#include <cstdint>

namespace Texture {

void Initialize() {
    // The texture reuse pool only has buckets for 32-512 (5x5). 1024 sized textures bypass the pool
    // and are created directly (no leak, just less reuse). A proper fix is to extend the pool to
    // 32-1024 (6x6), or remap the buckets to 64-1024.
    const uint8_t newByte = 0x87;
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXTURE_SIZE_1), &newByte, 1);
    Common::PatchBytes(reinterpret_cast<void *>(Offsets::PATCH_TEXTURE_SIZE_2), &newByte, 1);
}

} // namespace Texture
