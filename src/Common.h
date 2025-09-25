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

namespace Common {

#define HOOK_FUNCTION(offset, hook, original)                                                      \
    {                                                                                              \
        auto *target = reinterpret_cast<LPVOID>(offset);                                           \
        if (MH_CreateHook(target, static_cast<LPVOID>(hook),                                       \
                          reinterpret_cast<LPVOID *>(&original)) != MH_OK)                         \
            return FALSE;                                                                          \
        if (MH_EnableHook(target) != MH_OK)                                                        \
            return FALSE;                                                                          \
    }

} // namespace Common
