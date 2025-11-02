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

#include <memory>
#include <unordered_map>
#include <variant>

namespace Morph {

using factionValuesMap_t = std::unordered_map<uint32_t, std::vector<uint32_t>>;
using morphValue_t = std::variant<uint32_t, std::shared_ptr<factionValuesMap_t>>;

static std::unordered_map<uint64_t, std::unordered_map<uint32_t, morphValue_t>>
    g_overrides; // guid -> field -> newValue
static std::unordered_map<uint32_t, std::unordered_map<uint32_t, morphValue_t>>
    g_remap; // field -> oldValue -> newValue
static std::unordered_map<uint64_t, std::unordered_map<uint32_t, uint32_t>>
    g_original; // guid -> field -> original

static Game::CGObject_C_SetBlock_t CGObject_C_SetBlock_o = nullptr;
static Game::CGUnit_C_Destructor_t CGUnit_C_Destructor_o = nullptr;

static constexpr uint32_t VISIBLE_ITEM_STRIDE = 0xC;

static void ApplyField(Game::CGPlayer_C *unit, uint32_t fieldIndex);

static inline uint32_t *GetFieldSlot(Game::CGPlayer_C *unit, uint32_t fieldIndex) {
    switch (fieldIndex) {
    case Game::UNIT_FIELD_DISPLAYID:
        return &unit->m_data->m_unitData.m_displayId;
    case Game::UNIT_FIELD_NATIVEDISPLAYID:
        return &unit->m_data->m_unitData.m_nativeDisplayId;
    case Game::UNIT_FIELD_MOUNTDISPLAYID:
        return &unit->m_data->m_unitData.m_mountDisplayId;
    case Game::UNIT_FIELD_BYTES_0:
        return &unit->m_data->m_unitData.m_bytes0;
    case Game::PLAYER_VISIBLE_ITEM_1_0:
    case Game::PLAYER_VISIBLE_ITEM_2_0:
    case Game::PLAYER_VISIBLE_ITEM_3_0:
    case Game::PLAYER_VISIBLE_ITEM_4_0:
    case Game::PLAYER_VISIBLE_ITEM_5_0:
    case Game::PLAYER_VISIBLE_ITEM_6_0:
    case Game::PLAYER_VISIBLE_ITEM_7_0:
    case Game::PLAYER_VISIBLE_ITEM_8_0:
    case Game::PLAYER_VISIBLE_ITEM_9_0:
    case Game::PLAYER_VISIBLE_ITEM_10_0:
    case Game::PLAYER_VISIBLE_ITEM_11_0:
    case Game::PLAYER_VISIBLE_ITEM_12_0:
    case Game::PLAYER_VISIBLE_ITEM_13_0:
    case Game::PLAYER_VISIBLE_ITEM_14_0:
    case Game::PLAYER_VISIBLE_ITEM_15_0:
    case Game::PLAYER_VISIBLE_ITEM_16_0:
    case Game::PLAYER_VISIBLE_ITEM_17_0:
    case Game::PLAYER_VISIBLE_ITEM_18_0:
    case Game::PLAYER_VISIBLE_ITEM_19_0:
        return &unit->m_data->m_visibleItems[(fieldIndex - Game::PLAYER_VISIBLE_ITEM_1_0) / 0xC]
                    .data[0];
    default:
        return nullptr;
    }
}

template <typename T>
static inline void EraseNested(std::unordered_map<uint64_t, std::unordered_map<uint32_t, T>> &m,
                               uint64_t k1, uint32_t k2) {
    auto it = m.find(k1);
    if (it == m.end())
        return;
    it->second.erase(k2);
    if (it->second.empty())
        m.erase(it);
}

static inline bool GetOverrideValue(uint64_t guid, uint32_t fieldIndex, morphValue_t &out) {
    const auto itOverrides = g_overrides.find(guid);
    if (itOverrides == g_overrides.end()) {
        return false;
    }
    const auto itOverridesField = itOverrides->second.find(fieldIndex);
    if (itOverridesField == itOverrides->second.end()) {
        return false;
    }
    out = itOverridesField->second;
    return true;
}

static inline bool GetRemapValue(uint32_t fieldIndex, uint32_t base, morphValue_t &out) {
    const auto itRemap = g_remap.find(fieldIndex);
    if (itRemap == g_remap.end())
        return false;
    const auto itRemapValues = itRemap->second.find(base);
    if (itRemapValues == itRemap->second.end())
        return false;
    out = itRemapValues->second;
    return true;
}

static inline bool UsingNativeDisplay(Game::CGPlayer_C *unit) {
    const uint64_t guid = unit->m_data->m_unitData.m_objectData.m_guid;

    morphValue_t value;
    if (GetOverrideValue(guid, Game::UNIT_FIELD_DISPLAYID, value)) {
        return std::get<uint32_t>(value) == 0;
    }

    const auto itOriginal = g_original.find(guid);
    if (itOriginal != g_original.end()) {
        const auto itOriginalFields = itOriginal->second.find(Game::UNIT_FIELD_DISPLAYID);
        if (itOriginalFields != itOriginal->second.end()) {
            if (GetRemapValue(Game::UNIT_FIELD_DISPLAYID, itOriginalFields->second, value)) {
                return std::get<uint32_t>(value) == 0;
            }
        }
    }
    return false;
}

static inline bool EvaluateMorphValue(Game::CGPlayer_C *unit, uint32_t fieldIndex,
                                      const morphValue_t &morphValue, uint32_t &outValue) {
    if (fieldIndex == Game::UNIT_FIELD_DISPLAYID && std::get<uint32_t>(morphValue) == 0) {
        outValue = unit->m_data ? unit->m_data->m_unitData.m_nativeDisplayId : 0;
        return true;
    } else if (std::holds_alternative<std::shared_ptr<factionValuesMap_t>>(morphValue)) {
        auto factionReplacementMap = std::get<std::shared_ptr<factionValuesMap_t>>(morphValue);
        const auto *factionTemplate = Game::DbLookupById<Game::FactionTemplate>(
            Game::g_factionTemplateDB, unit->m_data->m_unitData.m_factionTemplate);

        if (factionTemplate) {
            const auto it = factionReplacementMap->find(factionTemplate->m_faction);
            if (it != factionReplacementMap->end() && !it->second.empty()) {
                outValue =
                    it->second[unit->m_data->m_unitData.m_objectData.m_guid % it->second.size()];
                return true;
            }
        }
        const auto it = factionReplacementMap->find(0);
        if (it != factionReplacementMap->end() && !it->second.empty()) {
            outValue = it->second[unit->m_data->m_unitData.m_objectData.m_guid % it->second.size()];
            return true;
        }
        return false;
    } else {
        outValue = std::get<uint32_t>(morphValue);
        return true;
    }
}

static inline bool EvaluateWriteReplacement(Game::CGPlayer_C *unit, uint32_t fieldIndex,
                                            uint32_t incoming, uint32_t &outValue) {
    const uint64_t guid = unit->m_data->m_unitData.m_objectData.m_guid;
    morphValue_t value;
    if (GetOverrideValue(guid, fieldIndex, value)) {
        if (EvaluateMorphValue(unit, fieldIndex, value, outValue)) {
            g_original[guid][fieldIndex] = incoming;
            return true;
        }
    }

    if (GetRemapValue(fieldIndex, incoming, value)) {
        if (EvaluateMorphValue(unit, fieldIndex, value, outValue)) {
            g_original[guid][fieldIndex] = incoming;
            return true;
        }
    }

    EraseNested(g_original, guid, fieldIndex);
    return false;
}

static void OnDisplayIDChanged(Game::CGPlayer_C *unit, uint32_t newValue) {
    const auto itOriginal = g_original.find(unit->m_data->m_unitData.m_objectData.m_guid);
    if (itOriginal != g_original.end()) {
        const auto itOriginalFields = itOriginal->second.find(Game::UNIT_FIELD_DISPLAYID);
        if (itOriginalFields != itOriginal->second.end()) {
            uint8_t race = 0;
            uint8_t gender = 0;
            switch (newValue) {
            case Game::RacialModel::MODEL_HUMAN_MALE:
                race = Game::Races::RACE_HUMAN;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_HUMAN_FEMALE:
                race = Game::Races::RACE_HUMAN;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_ORC_MALE:
                race = Game::Races::RACE_ORC;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_ORC_FEMALE:
                race = Game::Races::RACE_ORC;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_DWARF_MALE:
                race = Game::Races::RACE_DWARF;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_DWARF_FEMALE:
                race = Game::Races::RACE_DWARF;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_NELF_MALE:
                race = Game::Races::RACE_NIGHTELF;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_NELF_FEMALE:
                race = Game::Races::RACE_NIGHTELF;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_UNDEAD_MALE:
                race = Game::Races::RACE_UNDEAD;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_UNDEAD_FEMALE:
                race = Game::Races::RACE_UNDEAD;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_TAUREN_MALE:
                race = Game::Races::RACE_TAUREN;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_TAUREN_FEMALE:
                race = Game::Races::RACE_TAUREN;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_TROLL_MALE:
                race = Game::Races::RACE_TROLL;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_TROLL_FEMALE:
                race = Game::Races::RACE_TROLL;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_GNOME_MALE:
                race = Game::Races::RACE_GNOME;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_GNOME_FEMALE:
                race = Game::Races::RACE_GNOME;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_GOBLIN_MALE:
                race = Game::Races::RACE_GOBLIN;
                gender = Game::Gender::GENDER_MALE;
                break;
            case Game::RacialModel::MODEL_GOBLIN_FEMALE:
                race = Game::Races::RACE_GOBLIN;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_HELF_FEMALE:
                race = Game::Races::RACE_HIGH_ELF;
                gender = Game::Gender::GENDER_FEMALE;
                break;
            case Game::RacialModel::MODEL_HELF_MALE:
                race = Game::Races::RACE_HIGH_ELF;
                gender = Game::Gender::GENDER_MALE;
                break;
            default:
                break;
            }

            if (race > 0) {
                uint32_t bytes0 = unit->m_data->m_unitData.m_bytes0;
                bytes0 = (bytes0 & 0xFFFFFF00U) | uint32_t(race);
                bytes0 = (bytes0 & 0xFF00FFFFU) | (uint32_t(gender) << 16);
                g_overrides[unit->m_data->m_unitData.m_objectData.m_guid]
                           [Game::UNIT_FIELD_BYTES_0] = bytes0;
                g_overrides[unit->m_data->m_unitData.m_objectData.m_guid]
                           [Game::UNIT_FIELD_NATIVEDISPLAYID] = newValue;
                ApplyField(unit, Game::UNIT_FIELD_BYTES_0);
                ApplyField(unit, Game::UNIT_FIELD_NATIVEDISPLAYID);
                return;
            }
        }
    }
    morphValue_t value;
    if (GetOverrideValue(unit->m_data->m_unitData.m_objectData.m_guid, Game::UNIT_FIELD_BYTES_0,
                         value)) {
        EraseNested(g_overrides, unit->m_data->m_unitData.m_objectData.m_guid,
                    Game::UNIT_FIELD_BYTES_0);
        EraseNested(g_overrides, unit->m_data->m_unitData.m_objectData.m_guid,
                    Game::UNIT_FIELD_NATIVEDISPLAYID);
        ApplyField(unit, Game::UNIT_FIELD_BYTES_0);
        ApplyField(unit, Game::UNIT_FIELD_NATIVEDISPLAYID);
    }
}

static inline void OnFieldChanging(Game::CGPlayer_C *unit, uint32_t fieldIndex, uint32_t newValue) {
    switch (fieldIndex) {
    case Game::UNIT_FIELD_DISPLAYID:
        OnDisplayIDChanged(unit, newValue);
        break;
    }
}

static void ReloadUnitDisplay(Game::CGPlayer_C *unit) {
    uint32_t original = unit->m_data->m_unitData.m_displayId;
    unit->m_data->m_unitData.m_displayId = 15435;
    Game::CGUnit_C_UpdateDisplayInfo(reinterpret_cast<Game::CGUnit_C *>(unit));
    unit->m_data->m_unitData.m_displayId = original;
    Game::CGUnit_C_UpdateDisplayInfo(reinterpret_cast<Game::CGUnit_C *>(unit));
}

static inline void OnFieldChanged(Game::CGPlayer_C *unit, uint32_t fieldIndex) {
    switch (fieldIndex) {
    case Game::UNIT_FIELD_DISPLAYID:
        Game::CGUnit_C_UpdateDisplayInfo(reinterpret_cast<Game::CGUnit_C *>(unit));
        break;
    case Game::UNIT_FIELD_MOUNTDISPLAYID:
        Game::CGUnit_C_RefreshMount(reinterpret_cast<Game::CGUnit_C *>(unit), true);
        break;
    case Game::PLAYER_VISIBLE_ITEM_1_0:
    case Game::PLAYER_VISIBLE_ITEM_2_0:
    case Game::PLAYER_VISIBLE_ITEM_3_0:
    case Game::PLAYER_VISIBLE_ITEM_4_0:
    case Game::PLAYER_VISIBLE_ITEM_5_0:
    case Game::PLAYER_VISIBLE_ITEM_6_0:
    case Game::PLAYER_VISIBLE_ITEM_7_0:
    case Game::PLAYER_VISIBLE_ITEM_8_0:
    case Game::PLAYER_VISIBLE_ITEM_9_0:
    case Game::PLAYER_VISIBLE_ITEM_10_0:
    case Game::PLAYER_VISIBLE_ITEM_11_0:
    case Game::PLAYER_VISIBLE_ITEM_12_0:
    case Game::PLAYER_VISIBLE_ITEM_13_0:
    case Game::PLAYER_VISIBLE_ITEM_14_0:
    case Game::PLAYER_VISIBLE_ITEM_15_0:
    case Game::PLAYER_VISIBLE_ITEM_16_0:
    case Game::PLAYER_VISIBLE_ITEM_17_0:
    case Game::PLAYER_VISIBLE_ITEM_18_0:
    case Game::PLAYER_VISIBLE_ITEM_19_0:
        // The proper way would be to call CGPlayer_C::UpdateInventoryComponent at 0x5DEE30 or at
        // least replicate it.
        ReloadUnitDisplay(unit);
        break;
    }
}

static void ApplyField(Game::CGPlayer_C *unit, uint32_t fieldIndex) {
    uint32_t *slot = GetFieldSlot(unit, fieldIndex);
    if (!slot)
        return;

    const uint64_t guid = unit->m_data->m_unitData.m_objectData.m_guid;
    const uint32_t current = *slot;

    uint32_t base = current;
    if (const auto itOriginal = g_original.find(guid); itOriginal != g_original.end()) {
        const auto itOriginalFields = itOriginal->second.find(fieldIndex);
        if (itOriginalFields != itOriginal->second.end())
            base = itOriginalFields->second;
    }

    morphValue_t value;
    bool haveOverride = GetOverrideValue(guid, fieldIndex, value);
    bool haveRemap = false;
    if (!haveOverride) {
        haveRemap = GetRemapValue(fieldIndex, base, value);
    }

    uint32_t evaluatedValue;
    if ((haveOverride || haveRemap) &&
        EvaluateMorphValue(unit, fieldIndex, value, evaluatedValue)) {
        if (g_original[guid].find(fieldIndex) == g_original[guid].end()) {
            g_original[guid][fieldIndex] = current;
        }
        if (current != evaluatedValue) {
            OnFieldChanging(unit, fieldIndex, evaluatedValue);
            *slot = evaluatedValue;
            OnFieldChanged(unit, fieldIndex);
        }
        return;
    }

    if (const auto itOriginal = g_original.find(guid); itOriginal != g_original.end()) {
        const auto itOriginalFields = itOriginal->second.find(fieldIndex);
        if (itOriginalFields != itOriginal->second.end()) {
            uint32_t original = itOriginalFields->second;
            EraseNested(g_original, guid, fieldIndex);
            if (current != original) {
                OnFieldChanging(unit, fieldIndex, original);
                *slot = original;
                OnFieldChanged(unit, fieldIndex);
            }
        }
    }
}

static int __fastcall CGObject_C_SetBlock_h(Game::CGObject_C *thisptr, void * /*edx*/,
                                            uint32_t fieldIndex, uint32_t value) {
    if (thisptr->m_objectType != Game::OBJECT_TYPE::PLAYER) {
        return CGObject_C_SetBlock_o(thisptr, fieldIndex, value);
    }

    auto *unit = reinterpret_cast<Game::CGPlayer_C *>(thisptr);

    if (fieldIndex == Game::UNIT_FIELD_NATIVEDISPLAYID && UsingNativeDisplay(unit)) {
        unit->m_data->m_unitData.m_displayId = value;
    }

    if (GetFieldSlot(unit, fieldIndex) != nullptr) {
        uint32_t rewritten;
        if (EvaluateWriteReplacement(unit, fieldIndex, value, rewritten)) {
            value = rewritten;
            OnFieldChanging(unit, fieldIndex, value);
        }
    }

    return CGObject_C_SetBlock_o(thisptr, fieldIndex, value);
}

static void __fastcall CGUnit_C_Destructor_h(Game::CGUnit_C *thisptr) {
    g_original.erase(thisptr->m_guid);
    CGUnit_C_Destructor_o(thisptr);
}

static bool __fastcall RefreshFieldCallback(void *context, void * /*edx*/, uint64_t guid) {
    auto *unit = reinterpret_cast<Game::CGPlayer_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_PLAYER, nullptr, guid, 0));
    if (!unit)
        return true;

    if (context == nullptr) {
        ApplyField(unit, Game::UNIT_FIELD_DISPLAYID);
        ApplyField(unit, Game::UNIT_FIELD_MOUNTDISPLAYID);
        for (uint32_t i = 0; i < 19; ++i) {
            ApplyField(unit, Game::PLAYER_VISIBLE_ITEM_1_0 + (i * VISIBLE_ITEM_STRIDE));
        }
    } else {
        ApplyField(unit, reinterpret_cast<uint32_t>(context));
    }
    return true;
}

static std::shared_ptr<factionValuesMap_t> ParseFactionValuesTable(void *L, int index) {
    auto factionValues = std::make_shared<factionValuesMap_t>();

    Game::Lua::PushNil(L);
    while (Game::Lua::Next(L, index) != 0) {
        if (Game::Lua::IsNumber(L, -2) && Game::Lua::IsTable(L, -1)) {
            auto replacementList = std::vector<uint32_t>();
            Game::Lua::PushNil(L);
            while (Game::Lua::Next(L, -2)) {
                if (Game::Lua::IsNumber(L, -1)) {
                    replacementList.push_back(static_cast<uint32_t>(Game::Lua::ToNumber(L, -1)));
                }
                Game::Lua::Pop(L, 1);
            }
            factionValues->emplace(static_cast<uint32_t>(Game::Lua::ToNumber(L, -2)),
                                   replacementList);
        }
        Game::Lua::Pop(L, 1);
    }

    return factionValues;
}

static int __fastcall SetUnitFieldImpl(void *L, uint32_t fieldId, const char *usage) {
    if (!Game::Lua::IsString(L, 1)) {
        Game::Lua::Error(L, usage);
        return 0;
    }

    const char *unitToken = Game::Lua::ToString(L, 1);
    const uint64_t guid = Game::GetGUIDFromName(unitToken);
    if (guid == 0) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    auto *unit = reinterpret_cast<Game::CGPlayer_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_PLAYER, nullptr, guid, 0));

    if (unit == nullptr) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    if (!Game::Lua::IsNumber(L, 2)) {
        EraseNested(g_overrides, guid, fieldId);
        ApplyField(unit, fieldId);
        return 0;
    }

    const auto value = static_cast<uint32_t>(Game::Lua::ToNumber(L, 2));
    g_overrides[guid][fieldId] = value;
    ApplyField(unit, fieldId);
    return 0;
}

static int __fastcall RemapFieldImpl(void *L, uint32_t fieldId, const char *usage) {
    if (!Game::Lua::IsNumber(L, 1) && !Game::Lua::IsTable(L, 1)) {
        Game::Lua::Error(L, usage);
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
        Game::Lua::Error(L, usage);
        return 0;
    }

    if (!Game::Lua::IsNumber(L, 2)) {
        for (auto oldId : oldIds) {
            const auto itRemap = g_remap.find(fieldId);
            if (itRemap != g_remap.end()) {
                itRemap->second.erase(oldId);
                if (itRemap->second.empty())
                    g_remap.erase(itRemap);
            }
        }
        Game::ClntObjMgrEnumVisibleObjects(RefreshFieldCallback, reinterpret_cast<void *>(fieldId));
        return 0;
    }

    const auto newId = static_cast<uint32_t>(Game::Lua::ToNumber(L, 2));
    for (auto oldId : oldIds) {
        g_remap[fieldId][oldId] = newId;
    }
    Game::ClntObjMgrEnumVisibleObjects(RefreshFieldCallback, reinterpret_cast<void *>(fieldId));
    return 0;
}

static int __fastcall Script_SetUnitDisplayID(void *L) {
    return SetUnitFieldImpl(L, Game::UNIT_FIELD_DISPLAYID,
                            "Usage: SetUnitDisplayID(unitToken [, displayID])");
}

static int __fastcall Script_RemapDisplayID(void *L) {
    return RemapFieldImpl(L, Game::UNIT_FIELD_DISPLAYID,
                          "Usage: RemapDisplayID(oldDisplayID(s) [, newDisplayID])");
}

static int __fastcall Script_SetUnitMountDisplayID(void *L) {
    return SetUnitFieldImpl(L, Game::UNIT_FIELD_MOUNTDISPLAYID,
                            "Usage: SetUnitMountDisplayID(unitToken [, mountDisplayID])");
}

static int __fastcall Script_RemapMountDisplayID(void *L) {
    if (!Game::Lua::IsNumber(L, 1) && !Game::Lua::IsTable(L, 1)) {
        Game::Lua::Error(
            L, "Usage: RemapMountDisplayID(oldDisplayID(s) [, factionIndexedDisplayIDs])");
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
        Game::Lua::Error(
            L, "Usage: RemapMountDisplayID(oldDisplayID(s) [, factionIndexedDisplayIDs])");
        return 0;
    }

    if (!Game::Lua::IsTable(L, 2)) {
        for (auto oldId : oldIds) {
            const auto itRemap = g_remap.find(Game::UNIT_FIELD_MOUNTDISPLAYID);
            if (itRemap != g_remap.end()) {
                itRemap->second.erase(oldId);
                if (itRemap->second.empty())
                    g_remap.erase(itRemap);
            }
        }
        Game::ClntObjMgrEnumVisibleObjects(
            RefreshFieldCallback, reinterpret_cast<void *>(Game::UNIT_FIELD_MOUNTDISPLAYID));
        return 0;
    }

    auto factionMap = ParseFactionValuesTable(L, 2);
    if (factionMap->empty()) {
        Game::Lua::Error(
            L, "Usage: RemapMountDisplayID(oldDisplayID(s) [, factionIndexedDisplayIDs])");
        return 0;
    }

    for (auto oldId : oldIds) {
        g_remap[Game::UNIT_FIELD_MOUNTDISPLAYID][oldId] = factionMap;
    }
    Game::ClntObjMgrEnumVisibleObjects(RefreshFieldCallback,
                                       reinterpret_cast<void *>(Game::UNIT_FIELD_MOUNTDISPLAYID));
    return 0;
}

static int __fastcall Script_SetUnitVisibleItemID(void *L) {
    if (!Game::Lua::IsString(L, 1) || !Game::Lua::IsNumber(L, 2)) {
        Game::Lua::Error(L, "Usage: SetUnitVisibleItemID(unitToken, inventorySlot [, displayID])");
        return 0;
    }

    const char *unitToken = Game::Lua::ToString(L, 1);
    const uint64_t guid = Game::GetGUIDFromName(unitToken);
    if (guid == 0) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    auto *unit = reinterpret_cast<Game::CGPlayer_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_PLAYER, nullptr, guid, 0));

    if (unit == nullptr) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    const auto slot = static_cast<uint32_t>(Game::Lua::ToNumber(L, 2));
    if (slot < 1 || slot > 19) {
        Game::Lua::Error(L, "Unknown inventory slot.");
        return 0;
    }

    const uint32_t fieldId = Game::PLAYER_VISIBLE_ITEM_1_0 + ((slot - 1) * VISIBLE_ITEM_STRIDE);

    if (!Game::Lua::IsNumber(L, 3)) {
        EraseNested(g_overrides, guid, fieldId);
        ApplyField(unit, fieldId);
        return 0;
    }

    const auto value = static_cast<uint32_t>(Game::Lua::ToNumber(L, 3));
    g_overrides[guid][fieldId] = value;
    ApplyField(unit, fieldId);
    return 0;
}

static int __fastcall Script_RemapVisibleItemID(void *L) {
    if (!(Game::Lua::IsNumber(L, 1) || Game::Lua::IsTable(L, 1)) || !Game::Lua::IsNumber(L, 2)) {
        Game::Lua::Error(
            L, "Usage: RemapVisibleItemID(oldDisplayID(s), inventorySlot [, newDisplayID])");
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
        Game::Lua::Error(
            L, "Usage: RemapVisibleItemID(oldDisplayID(s), inventorySlot, [, newDisplayID])");
        return 0;
    }

    const auto slot = static_cast<uint32_t>(Game::Lua::ToNumber(L, 2));
    if (slot < 1 || slot > 19) {
        Game::Lua::Error(L, "Unknown inventory slot.");
        return 0;
    }

    const uint32_t fieldId = Game::PLAYER_VISIBLE_ITEM_1_0 + ((slot - 1) * VISIBLE_ITEM_STRIDE);

    if (!Game::Lua::IsNumber(L, 3)) {
        for (auto oldId : oldIds) {
            const auto itRemap = g_remap.find(fieldId);
            if (itRemap != g_remap.end()) {
                itRemap->second.erase(oldId);
                if (itRemap->second.empty())
                    g_remap.erase(itRemap);
            }
        }
        Game::ClntObjMgrEnumVisibleObjects(RefreshFieldCallback, reinterpret_cast<void *>(fieldId));
        return 0;
    }

    const auto newId = static_cast<uint32_t>(Game::Lua::ToNumber(L, 3));
    for (auto oldId : oldIds) {
        g_remap[fieldId][oldId] = newId;
    }
    Game::ClntObjMgrEnumVisibleObjects(RefreshFieldCallback, reinterpret_cast<void *>(fieldId));
    return 0;
}

static int __fastcall Script_UnitDisplayInfo(void *L) {
    if (!Game::Lua::IsString(L, 1)) {
        Game::Lua::Error(L, "Usage: UnitDisplayInfo(unitToken)");
        return 0;
    }

    const char *unitToken = Game::Lua::ToString(L, 1);
    const uint64_t guid = Game::GetGUIDFromName(unitToken);
    if (guid == 0) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    auto *unit = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_UNIT, nullptr, guid, 0));
    if (unit == nullptr) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    Game::Lua::PushNumber(L, unit->m_data->m_displayId);
    Game::Lua::PushNumber(L, unit->m_data->m_nativeDisplayId);
    Game::Lua::PushNumber(L, unit->m_data->m_mountDisplayId);

    return 3;
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
    Game::FrameScript_RegisterFunction("SetUnitMountDisplayID",
                                       reinterpret_cast<uintptr_t>(&Script_SetUnitMountDisplayID));
    Game::FrameScript_RegisterFunction("RemapMountDisplayID",
                                       reinterpret_cast<uintptr_t>(&Script_RemapMountDisplayID));
    Game::FrameScript_RegisterFunction("SetUnitVisibleItemID",
                                       reinterpret_cast<uintptr_t>(&Script_SetUnitVisibleItemID));
    Game::FrameScript_RegisterFunction("RemapVisibleItemID",
                                       reinterpret_cast<uintptr_t>(&Script_RemapVisibleItemID));
    Game::FrameScript_RegisterFunction("UnitDisplayInfo",
                                       reinterpret_cast<uintptr_t>(&Script_UnitDisplayInfo));
}

void Reset() {
    g_overrides.clear();
    g_remap.clear();
    if (Game::ClntObjMgrGetActivePlayer() > 0) {
        Game::ClntObjMgrEnumVisibleObjects(RefreshFieldCallback, nullptr);
    }
}

} // namespace Morph
