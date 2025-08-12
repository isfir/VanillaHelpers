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

#pragma once

#include "../Game.h"

#include <cstdint>

namespace Game {

// Contains the data used to draw the blips rendered in RenderObjectBlips
struct MinimapObject {
    uint64_t guid;
    C2Vector minimapPosition; // 0,0 is bottom left
    char isInDifferentArea;   // Controls showing grayed blip
    char unk[7];              // Probably padding
};

struct MinimapObjectArray {
    uint32_t capacity;
    uint32_t count;
    MinimapObject *data;
    uint32_t unk;
};

// Contains the scale of each of the original blips
struct MinimapTypeInfoEntry {
    float scale;
    uint32_t unk;
};

// Used in the original RenderObjectBlips
typedef int(__cdecl *atexit_t)(void(__cdecl *)());
// Used to get the world radius given to WorldPosToMinimapFrameCoords, but it's already in the
// MINIMAPINFO passed to RenderObjectBlips
typedef float(__cdecl *MinimapGetWorldRadius_t)();
// Used in the original RenderObjectBlips
extern atexit_t atexit;
// Used to get the world radius given to WorldPosToMinimapFrameCoords, but it's already in the
// MINIMAPINFO passed to RenderObjectBlips
extern MinimapGetWorldRadius_t MinimapGetWorldRadius;

// Used in the original RenderObjectBlips
extern uint8_t &s_renderObjectBlipsInitialized;
// Used by the original RenderObjectBlips, this is the default atlas texture (ObjectIcons.blp)
extern HTEXTURE__ *&s_blipTexture;
// Contains the data used to draw the blips rendered in RenderObjectBlips
extern MinimapObjectArray *s_minimapObjects;
// Contains the texture coords of the original blip atlas texture (ObjectIcons.blp)
extern TexCoord *s_iconCoords;
// Contains the scale of each of the original blips
extern MinimapTypeInfoEntry *s_minimapTypeInfo;

} // namespace Game
