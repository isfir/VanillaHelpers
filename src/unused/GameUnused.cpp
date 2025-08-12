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

#include "GameUnused.h"
#include "OffsetsUnused.h"

namespace Game {
// Used in the original RenderObjectBlips
atexit_t atexit = reinterpret_cast<atexit_t>(Offsets::FUN_ATEXIT);
// Used to get the world radius given to WorldPosToMinimapFrameCoords, but it's already in the
// MINIMAPINFO passed to RenderObjectBlips
MinimapGetWorldRadius_t MinimapGetWorldRadius =
    reinterpret_cast<MinimapGetWorldRadius_t>(Offsets::FUN_MINIMAP_GET_WORLD_RADIUS);

// Used in the original RenderObjectBlips
uint8_t &s_renderObjectBlipsInitialized =
    *reinterpret_cast<uint8_t *>(Offsets::VAR_RENDER_OBJECT_BLIPS_INITIALIZED);
// Used by the original RenderObjectBlips, this is the default atlas texture (ObjectIcons.blp)
HTEXTURE__ *&s_blipTexture = *reinterpret_cast<HTEXTURE__ **>(Offsets::VAR_BLIP_TEXTURE);
// Contains the data used to draw the blips rendered in RenderObjectBlips
MinimapObjectArray *s_minimapObjects =
    reinterpret_cast<MinimapObjectArray *>(Offsets::VAR_MINIMAP_OBJECTS);
// Contains the texture coords of the original blip atlas texture (ObjectIcons.blp)
TexCoord *s_iconCoords = reinterpret_cast<TexCoord *>(Offsets::CONST_ICON_COORDS);
// Contains the scale of each of the original blips
MinimapTypeInfoEntry *s_minimapTypeInfo =
    reinterpret_cast<MinimapTypeInfoEntry *>(Offsets::CONST_MINI_MAP_TYPE_INFO);

} // namespace Game
