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

enum Offsets {
    FUN_ATEXIT = 0x409AEF,
    FUN_MINIMAP_GET_WORLD_RADIUS = 0x6dab70,

    CONST_ICON_COORDS = 0xBC7898,
    CONST_MINI_MAP_TYPE_INFO = 0x84C4EC,

    VAR_BLIP_TEXTURE = 0xBC822C,
    VAR_MINIMAP_OBJECTS = 0xBC82B0,
    VAR_RENDER_OBJECT_BLIPS_INITIALIZED = 0xBC7634,
};
