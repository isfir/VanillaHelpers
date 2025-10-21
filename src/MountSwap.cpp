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

#include "MountSwap.h"
#include "Common.h"
#include "Game.h"
#include "MinHook.h"
#include "Offsets.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace MountSwap {

static Game::CGUnit_C_CreateUnitMount_t CGUnit_C_CreateUnitMount_o = nullptr;

static std::unordered_set<uint32_t> g_swappedMountIds;
static std::mutex g_swappedMountIdsMutex;

static std::unordered_map<uint32_t, std::vector<uint32_t>> g_replacementMountIdsByFaction;
static std::mutex g_replacementMountsMutex;

static bool g_swapsEnabled = false;

static void ParseDisplayIdListOrError(void *L, const std::string &csv,
                                      std::vector<uint32_t> &outIds);
static uint32_t ParseFactionId(void *L, int index, const char *usageMessage);

static bool IsSwappedMountId(uint32_t displayId) {
    std::lock_guard<std::mutex> lock(g_swappedMountIdsMutex);
    return g_swappedMountIds.count(displayId) != 0;
}

static uint32_t SelectReplacementId(uint32_t factionId, uint64_t guid) {
    std::lock_guard<std::mutex> lock(g_replacementMountsMutex);
    const auto it = g_replacementMountIdsByFaction.find(factionId);
    if (it == g_replacementMountIdsByFaction.end() || it->second.empty())
        return 0;

    const auto &replacementIds = it->second;
    const size_t idx = static_cast<size_t>(guid % replacementIds.size());
    return replacementIds[idx];
}

static void __fastcall CGUnit_C_CreateUnitMount_h(Game::CGUnit_C *unitptr) {
    const uint32_t originalDisplayId = unitptr->m_data->m_mountDisplayId;
    const bool shouldSwapMount = g_swapsEnabled && IsSwappedMountId(originalDisplayId);

    if (shouldSwapMount) {
        const auto *factionTemplate = Game::DbLookupById<Game::FactionTemplate>(
            Game::g_factionTemplateDB, unitptr->m_data->m_factionTemplate);

        if (factionTemplate) {
            const uint32_t replacementDisplayId =
                SelectReplacementId(factionTemplate->m_faction, unitptr->m_guid);
            if (replacementDisplayId != 0) {
                unitptr->m_data->m_mountDisplayId = replacementDisplayId;
                CGUnit_C_CreateUnitMount_o(unitptr);
                unitptr->m_data->m_mountDisplayId = originalDisplayId;
                return;
            }
        }
    }

    CGUnit_C_CreateUnitMount_o(unitptr);
}

static bool __fastcall RefreshMountsCallback(void * /*context*/, void *edx, uint64_t guid) {
    auto *unitptr = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_UNIT, nullptr, guid, 0));
    if (unitptr == nullptr)
        return true;

    uint32_t mountDisplayId = unitptr->m_data->m_mountDisplayId;

    // WARNING: We only refresh if the mount is flagged as swappable (rather then all on screen)
    // We need to be careful to ensure that we do not lose references to mounts we originally changed.
    if (mountDisplayId > 0 && IsSwappedMountId(mountDisplayId)) {
        Game::CGUnit_C_RefreshMount(unitptr, true);
    }

    return true;
}

static void SetSwapsEnabled(bool enabled) {
    g_swapsEnabled = enabled;

    if (Game::ClntObjMgrGetActivePlayer() > 0) {
        Game::ClntObjMgrEnumVisibleObjects(RefreshMountsCallback, nullptr);
    }
}

static uint32_t ParseDisplayIdArg(void *L, int index, const char *usageMessage) {
    if (!Game::Lua::IsNumber(L, index)) {
        Game::Lua::Error(L, usageMessage);
        return 0;
    }

    const double rawValue = Game::Lua::ToNumber(L, index);
    if (rawValue < 0.0 ||
        rawValue > static_cast<double>((std::numeric_limits<uint32_t>::max)()) ||
        std::floor(rawValue) != rawValue) {
        Game::Lua::Error(L, "Display ID must be an integer between 0 and 4294967295.");
        return 0;
    }

    return static_cast<uint32_t>(rawValue);
}

static int __fastcall Script_SetSwapsEnabled(void *L) {
    bool enabled = false;

    if (Game::Lua::IsNumber(L, 1)) {
        enabled = Game::Lua::ToNumber(L, 1) != 0.0;
    } else if (Game::Lua::IsString(L, 1)) {
        std::string value = Game::Lua::ToString(L, 1);
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (value == "1" || value == "true") {
            enabled = true;
        } else if (value == "0" || value == "false") {
            enabled = false;
        } else {
            Game::Lua::Error(L, "Usage: VH_Mounts_SetEnabled(value)");
            return 0;
        }
    } else {
        Game::Lua::Error(L, "Usage: VH_Mounts_SetEnabled(value)");
        return 0;
    }

    SetSwapsEnabled(enabled);
    return 0;
}

static int __fastcall Script_AddSwappedId(void *L) {
    const uint32_t displayId = ParseDisplayIdArg(L, 1, "Usage: VH_Mounts_AddSwappedId(displayId)");

    {
        std::lock_guard<std::mutex> lock(g_swappedMountIdsMutex);
        g_swappedMountIds.insert(displayId);
    }

    return 0;
}

static int __fastcall Script_GetReplacementIds(void *L) {
    const uint32_t factionId =
        ParseFactionId(L, 1, "Usage: VH_Mounts_GetReplacementIds(factionId)");

    std::vector<uint32_t> ids;
    {
        std::lock_guard<std::mutex> lock(g_replacementMountsMutex);
        const auto it = g_replacementMountIdsByFaction.find(factionId);
        if (it != g_replacementMountIdsByFaction.end())
            ids = it->second;
    }

    std::string joined;
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i != 0)
            joined.push_back(',');
        joined += std::to_string(ids[i]);
    }

    Game::Lua::PushString(L, joined.c_str());
    return 1;
}

static int __fastcall Script_RemoveSwappedId(void *L) {
    const uint32_t displayId =
        ParseDisplayIdArg(L, 1, "Usage: VH_Mounts_RemoveSwappedId(displayId)");

    {
        std::lock_guard<std::mutex> lock(g_swappedMountIdsMutex);
        g_swappedMountIds.erase(displayId);
    }

    return 0;
}

static int __fastcall Script_ResetSwappedIds(void * /*L*/) {
    std::lock_guard<std::mutex> lock(g_swappedMountIdsMutex);
    g_swappedMountIds.clear();

    return 0;
}

static int __fastcall Script_GetSwappedIds(void *L) {
    std::vector<uint32_t> ids;
    {
        std::lock_guard<std::mutex> lock(g_swappedMountIdsMutex);
        ids.assign(g_swappedMountIds.begin(), g_swappedMountIds.end());
    }

    std::sort(ids.begin(), ids.end());

    std::string joined;
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i != 0)
            joined.push_back(',');
        joined += std::to_string(ids[i]);
    }

    Game::Lua::PushString(L, joined.c_str());
    return 1;
}

static int __fastcall Script_SetSwappedIds(void *L) {
    if (!Game::Lua::IsString(L, 1)) {
        Game::Lua::Error(L, "Usage: VH_Mounts_SetSwappedIds(\"id1,id2,...\")");
        return 0;
    }

    const std::string csv = Game::Lua::ToString(L, 1);
    std::vector<uint32_t> ids;
    ParseDisplayIdListOrError(L, csv, ids);

    {
        std::lock_guard<std::mutex> lock(g_swappedMountIdsMutex);
        g_swappedMountIds.clear();
        g_swappedMountIds.insert(ids.begin(), ids.end());
    }

    return 0;
}

static void ParseDisplayIdListOrError(void *L, const std::string &csv,
                                      std::vector<uint32_t> &outIds) {
    std::stringstream stream(csv);
    std::string token;
    while (std::getline(stream, token, ',')) {
        const auto first = token.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
            continue;
        const auto last = token.find_last_not_of(" \t\r\n");
        const std::string trimmed = token.substr(first, last - first + 1);

        try {
            size_t idx = 0;
            const unsigned long value = std::stoul(trimmed, &idx, 10);
            if (idx != trimmed.size() ||
                value > static_cast<unsigned long>((std::numeric_limits<uint32_t>::max)())) {
                Game::Lua::Error(L, "Invalid display ID list.");
                return;
            }
            outIds.push_back(static_cast<uint32_t>(value));
        } catch (const std::exception &) {
            Game::Lua::Error(L, "Invalid display ID list.");
            return;
        }
    }

    std::sort(outIds.begin(), outIds.end());
    outIds.erase(std::unique(outIds.begin(), outIds.end()), outIds.end());
}

static uint32_t ParseFactionId(void *L, int index, const char *usageMessage) {
    if (!Game::Lua::IsNumber(L, index)) {
        Game::Lua::Error(L, usageMessage);
        return 0;
    }

    const double rawValue = Game::Lua::ToNumber(L, index);
    if (rawValue < 0.0 ||
        rawValue > static_cast<double>((std::numeric_limits<uint32_t>::max)()) ||
        std::floor(rawValue) != rawValue) {
        Game::Lua::Error(L, "Faction ID must be an integer between 0 and 4294967295.");
        return 0;
    }

    return static_cast<uint32_t>(rawValue);
}

static int __fastcall Script_SetReplacementIds(void *L) {
    if (!Game::Lua::IsNumber(L, 1) || !Game::Lua::IsString(L, 2)) {
        Game::Lua::Error(L,
                         "Usage: VH_Mounts_SetReplacementIds(factionId, \"id1,id2,...\")");
        return 0;
    }

    const uint32_t factionId =
        ParseFactionId(L, 1, "Usage: VH_Mounts_SetReplacementIds(factionId, \"id1,id2,...\")");
    const std::string csv = Game::Lua::ToString(L, 2);

    std::vector<uint32_t> replacementIds;
    ParseDisplayIdListOrError(L, csv, replacementIds);

    {
        std::lock_guard<std::mutex> lock(g_replacementMountsMutex);
        if (replacementIds.empty()) {
            g_replacementMountIdsByFaction.erase(factionId);
        } else {
            g_replacementMountIdsByFaction[factionId] = std::move(replacementIds);
        }
    }

    return 0;
}

static int __fastcall Script_ResetReplacementIds(void * /*L*/) {
    std::lock_guard<std::mutex> lock(g_replacementMountsMutex);
    g_replacementMountIdsByFaction.clear();
    return 0;
}

bool InstallHooks() {
    HOOK_FUNCTION(Offsets::FUN_CGUNIT_C_CREATE_UNIT_MOUNT, CGUnit_C_CreateUnitMount_h,
                  CGUnit_C_CreateUnitMount_o);

    return TRUE;
}
void Initialize() {
    g_swapsEnabled = false;

    {
        std::lock_guard<std::mutex> lock(g_swappedMountIdsMutex);
        g_swappedMountIds.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_replacementMountsMutex);
        g_replacementMountIdsByFaction.clear();
    }
}

void RegisterLuaFunctions() {
    // Register enable as a function rather then a var to allow for
    // resynchronization calls (via setting to enabled again while already enabled)
    Game::FrameScript_RegisterFunction("VH_Mounts_SetEnabled",
                                       reinterpret_cast<uintptr_t>(&Script_SetSwapsEnabled));
    Game::FrameScript_RegisterFunction("VH_Mounts_AddSwappedId",
                                       reinterpret_cast<uintptr_t>(&Script_AddSwappedId));
    Game::FrameScript_RegisterFunction("VH_Mounts_RemoveSwappedId",
                                       reinterpret_cast<uintptr_t>(&Script_RemoveSwappedId));
    Game::FrameScript_RegisterFunction("VH_Mounts_ResetSwappedIds",
                                       reinterpret_cast<uintptr_t>(&Script_ResetSwappedIds));
    Game::FrameScript_RegisterFunction("VH_Mounts_GetSwappedIds",
                                       reinterpret_cast<uintptr_t>(&Script_GetSwappedIds));
    Game::FrameScript_RegisterFunction("VH_Mounts_SetSwappedIds",
                                       reinterpret_cast<uintptr_t>(&Script_SetSwappedIds));
    Game::FrameScript_RegisterFunction("VH_Mounts_SetReplacementIds",
                                       reinterpret_cast<uintptr_t>(&Script_SetReplacementIds));
    Game::FrameScript_RegisterFunction("VH_Mounts_GetReplacementIds",
                                       reinterpret_cast<uintptr_t>(&Script_GetReplacementIds));
    Game::FrameScript_RegisterFunction("VH_Mounts_ResetReplacementIds",
                                       reinterpret_cast<uintptr_t>(&Script_ResetReplacementIds));
}

} // namespace MountSwap
