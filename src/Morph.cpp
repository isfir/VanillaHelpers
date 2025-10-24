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

#include "Morph.h"
#include "Common.h"
#include "Game.h"
#include "MinHook.h"
#include "Offsets.h"

#include <unordered_map>

namespace Morph {

enum UNIT_FIELDS : uint32_t {
    UNIT_FIELD_DISPLAYID = 0x83,
    UNIT_FIELD_NATIVEDISPLAYID = 0x84,
    UNIT_FIELD_MOUNTDISPLAYID = 0x85,
};

static std::unordered_map<uint64_t, std::unordered_map<uint32_t, uint32_t>>
    g_overrides; // guid -> field -> newValue
static std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>>
    g_remap; // field -> oldValue -> newValue
static std::unordered_map<uint64_t, std::unordered_map<uint32_t, uint32_t>>
    g_original; // guid -> field -> original

static Game::CGObject_C_SetBlock_t CGObject_C_SetBlock_o = nullptr;
static Game::CGUnit_C_Destructor_t CGUnit_C_Destructor_o = nullptr;

static inline uint32_t *GetFieldSlot(Game::CGUnit_C *unit, uint32_t fieldIndex) {
    if (!unit || !unit->m_data)
        return nullptr;
    switch (fieldIndex) {
    case UNIT_FIELD_DISPLAYID:
        return &unit->m_data->m_displayId;
    case UNIT_FIELD_NATIVEDISPLAYID:
        return &unit->m_data->m_nativeDisplayId;
    case UNIT_FIELD_MOUNTDISPLAYID:
        return &unit->m_data->m_mountDisplayId;
    default:
        return nullptr;
    }
}

static inline void OnFieldChanged(Game::CGUnit_C *unit, uint32_t fieldIndex) {
    switch (fieldIndex) {
    case UNIT_FIELD_DISPLAYID:
        Game::CGUnit_C_UpdateDisplayInfo(unit);
        break;
    case UNIT_FIELD_MOUNTDISPLAYID:
        Game::CGUnit_C_RefreshMount(unit, true);
        break;
    }
}

static inline void
EraseNested(std::unordered_map<uint64_t, std::unordered_map<uint32_t, uint32_t>> &m, uint64_t k1,
            uint32_t k2) {
    const auto it = m.find(k1);
    if (it == m.end())
        return;
    it->second.erase(k2);
    if (it->second.empty())
        m.erase(it);
}

static inline bool GetOverrideValue(uint64_t guid, uint32_t fieldIndex, uint32_t &out) {
    const auto itOverrides = g_overrides.find(guid);
    if (itOverrides == g_overrides.end())
        return false;
    const auto itOverridesField = itOverrides->second.find(fieldIndex);
    if (itOverridesField == itOverrides->second.end())
        return false;
    out = itOverridesField->second;
    return true;
}

static inline bool GetRemapValue(uint32_t fieldIndex, uint32_t base, uint32_t &out) {
    const auto itRemap = g_remap.find(fieldIndex);
    if (itRemap == g_remap.end())
        return false;
    const auto itRemapValues = itRemap->second.find(base);
    if (itRemapValues == itRemap->second.end())
        return false;
    out = itRemapValues->second;
    return true;
}

static inline bool UsingNativeDisplay(Game::CGUnit_C *unit) {
    const uint64_t guid = unit->m_guid;

    uint32_t value;
    if (GetOverrideValue(guid, UNIT_FIELD_DISPLAYID, value)) {
        return value == 0;
    }

    const auto itOriginal = g_original.find(guid);
    if (itOriginal != g_original.end()) {
        const auto itOriginalFields = itOriginal->second.find(UNIT_FIELD_DISPLAYID);
        if (itOriginalFields != itOriginal->second.end()) {
            if (GetRemapValue(UNIT_FIELD_DISPLAYID, itOriginalFields->second, value)) {
                return value == 0;
            }
        }
    }
    return false;
}

static inline bool EvaluateWriteReplacement(Game::CGUnit_C *unit, uint32_t fieldIndex,
                                            uint32_t incoming, uint32_t &outValue) {
    const uint64_t guid = unit->m_guid;

    uint32_t value;
    if (GetOverrideValue(guid, fieldIndex, value)) {
        g_original[guid][fieldIndex] = incoming;
        if (fieldIndex == UNIT_FIELD_DISPLAYID && value == 0) {
            value = unit->m_data ? unit->m_data->m_nativeDisplayId : 0;
        }
        outValue = value;
        return true;
    }

    if (GetRemapValue(fieldIndex, incoming, value)) {
        g_original[guid][fieldIndex] = incoming;
        if (fieldIndex == UNIT_FIELD_DISPLAYID && value == 0) {
            value = unit->m_data ? unit->m_data->m_nativeDisplayId : 0;
        }
        outValue = value;
        return true;
    }

    EraseNested(g_original, guid, fieldIndex);
    return false;
}

static void ApplyField(Game::CGUnit_C *unit, uint32_t fieldIndex) {
    uint32_t *slot = GetFieldSlot(unit, fieldIndex);
    if (!slot)
        return;

    const uint64_t guid = unit->m_guid;
    const uint32_t current = *slot;

    uint32_t base = current;
    if (const auto itOriginal = g_original.find(guid); itOriginal != g_original.end()) {
        const auto itOriginalFields = itOriginal->second.find(fieldIndex);
        if (itOriginalFields != itOriginal->second.end())
            base = itOriginalFields->second;
    }

    uint32_t value;
    bool haveOverride = GetOverrideValue(guid, fieldIndex, value);
    bool haveRemap = false;
    if (!haveOverride) {
        haveRemap = GetRemapValue(fieldIndex, base, value);
    }

    if (haveOverride || haveRemap) {
        if (g_original[guid].find(fieldIndex) == g_original[guid].end()) {
            g_original[guid][fieldIndex] = current;
        }
        if (fieldIndex == UNIT_FIELD_DISPLAYID && value == 0) {
            value = unit->m_data ? unit->m_data->m_nativeDisplayId : 0;
        }
        if (current != value) {
            *slot = value;
            OnFieldChanged(unit, fieldIndex);
        }
        return;
    }

    if (const auto itOriginal = g_original.find(guid); itOriginal != g_original.end()) {
        const auto itOriginalFields = itOriginal->second.find(fieldIndex);
        if (itOriginalFields != itOriginal->second.end()) {
            if (current != itOriginalFields->second) {
                *slot = itOriginalFields->second;
                OnFieldChanged(unit, fieldIndex);
            }
            EraseNested(g_original, guid, fieldIndex);
        }
    }
}

static int __fastcall CGObject_C_SetBlock_h(Game::CGObject_C *thisptr, void * /*edx*/, uint32_t fieldIndex,
                                     uint32_t value) {
    if (thisptr->m_objectType != Game::OBJECT_TYPE::PLAYER) {
        return CGObject_C_SetBlock_o(thisptr, fieldIndex, value);
    }

    auto *unit = reinterpret_cast<Game::CGUnit_C *>(thisptr);

    if (fieldIndex == UNIT_FIELD_NATIVEDISPLAYID && UsingNativeDisplay(unit)) {
        unit->m_data->m_displayId = unit->m_data->m_nativeDisplayId;
    }

    if (GetFieldSlot(unit, fieldIndex) != nullptr) {
        uint32_t rewritten;
        if (EvaluateWriteReplacement(unit, fieldIndex, value, rewritten)) {
            value = rewritten;
        }
    }

    return CGObject_C_SetBlock_o(thisptr, fieldIndex, value);
}

static void __fastcall CGUnit_C_Destructor_h(Game::CGUnit_C *thisptr) {
    g_original.erase(thisptr->m_guid);
    CGUnit_C_Destructor_o(thisptr);
}

static bool __fastcall RefreshDisplayCallback(void * /*context*/, void * /*edx*/, uint64_t guid) {
    auto *unit = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_PLAYER, nullptr, guid, 0));
    if (!unit)
        return true;
    ApplyField(unit, UNIT_FIELD_DISPLAYID);
    return true;
}

static int __fastcall Script_SetUnitDisplayID(void *L) {
    if (!Game::Lua::IsString(L, 1)) {
        Game::Lua::Error(L, "Usage: SetUnitDisplayID(unitName [, displayID])");
        return 0;
    }

    const char *name = Game::Lua::ToString(L, 1);
    const uint64_t guid = Game::GetGUIDFromName(name);
    if (guid == 0) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    auto *unit = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_PLAYER, nullptr, guid, 0));

    if (!Game::Lua::IsNumber(L, 2)) {
        EraseNested(g_overrides, guid, UNIT_FIELD_DISPLAYID);
        if (unit)
            ApplyField(unit, UNIT_FIELD_DISPLAYID);
        return 0;
    }

    const auto displayId = static_cast<uint32_t>(Game::Lua::ToNumber(L, 2)); // 0 => native
    g_overrides[guid][UNIT_FIELD_DISPLAYID] = displayId;
    if (unit) {
        ApplyField(unit, UNIT_FIELD_DISPLAYID);
    } else {
        Game::Lua::Error(L, "Player not found.");
    }
    return 0;
}

static int __fastcall Script_RemapDisplayID(void *L) {
    if (!Game::Lua::IsNumber(L, 1) && !Game::Lua::IsTable(L, 1)) {
        Game::Lua::Error(L, "Usage: RemapDisplayID(oldDisplayID(s) [, newDisplayID])");
        return 0;
    }

    auto oldIds = std::vector<uint32_t>();

    if (Game::Lua::IsNumber(L, 1)) {
        oldIds.push_back(static_cast<uint32_t>(Game::Lua::ToNumber(L, 1)));
    } else {
        Game::Lua::PushNil(L);
        while (Game::Lua::Next(L, 1) != 0) {
            if (Game::Lua::IsNumber(L, -1)) {
                oldIds.push_back(static_cast<uint32_t>(Game::Lua::ToNumber(L, -1)));
            }
            Game::Lua::Pop(L, 1);
        }
    }

    if (oldIds.empty()) {
        Game::Lua::Error(L, "Usage: RemapDisplayID(oldDisplayID(s) [, newDisplayID])");
        return 0;
    }

    if (!Game::Lua::IsNumber(L, 2)) {
        for (auto oldId : oldIds) {
            const auto itRemap = g_remap.find(UNIT_FIELD_DISPLAYID);
            if (itRemap != g_remap.end()) {
                itRemap->second.erase(oldId);
                if (itRemap->second.empty())
                    g_remap.erase(itRemap);
            }
        }
        Game::ClntObjMgrEnumVisibleObjects(RefreshDisplayCallback, nullptr);
        return 0;
    }

    const auto newId = static_cast<uint32_t>(Game::Lua::ToNumber(L, 2));
    for (auto oldId : oldIds) {
        g_remap[UNIT_FIELD_DISPLAYID][oldId] = newId;
    }
    Game::ClntObjMgrEnumVisibleObjects(RefreshDisplayCallback, nullptr);
    return 0;
}

static int __fastcall Script_UnitDisplayInfo(void *L) {
    if (!Game::Lua::IsString(L, 1)) {
        Game::Lua::Error(L, "Usage: GetUnitDisplayIDs(unitName)");
        return 0;
    }

    const char *name = Game::Lua::ToString(L, 1);
    const uint64_t guid = Game::GetGUIDFromName(name);
    if (guid == 0) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    auto *unit = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_UNIT, nullptr, guid, 0));
    if (!unit || !unit->m_data) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    Game::Lua::PushNumber(L, unit->m_data->m_nativeDisplayId);
    Game::Lua::PushNumber(L, unit->m_data->m_displayId);

    if (auto itOriginal = g_original.find(guid); itOriginal != g_original.end()) {
        auto itOriginalFields = itOriginal->second.find(UNIT_FIELD_DISPLAYID);
        if (itOriginalFields != itOriginal->second.end()) {
            Game::Lua::PushNumber(L, itOriginalFields->second);
        } else {
            Game::Lua::PushNil(L);
        }
    } else {
        Game::Lua::PushNil(L);
    }

    if (unit->m_data->m_mountDisplayId) {
        Game::Lua::PushNumber(L, unit->m_data->m_mountDisplayId);
    } else {
        Game::Lua::PushNil(L);
    }

    return 4;
}

bool InstallHooks() {
    HOOK_FUNCTION(Offsets::FUN_CGOBJECT_C_SET_BLOCK, CGObject_C_SetBlock_h, CGObject_C_SetBlock_o);
    HOOK_FUNCTION(Offsets::FUN_CGUNIT_C_DESTRUCTOR, CGUnit_C_Destructor_h, CGUnit_C_Destructor_o);
    return TRUE;
}

void RegisterLuaFunctions() {
    Game::FrameScript_RegisterFunction("SetUnitDisplayID",
                                       reinterpret_cast<uintptr_t>(&Script_SetUnitDisplayID));
    Game::FrameScript_RegisterFunction("RemapDisplayID",
                                       reinterpret_cast<uintptr_t>(&Script_RemapDisplayID));
    Game::FrameScript_RegisterFunction("UnitDisplayInfo",
                                       reinterpret_cast<uintptr_t>(&Script_UnitDisplayInfo));
}

void Initialize() {
    g_overrides.clear();
    g_remap.clear();
}

} // namespace Morph
