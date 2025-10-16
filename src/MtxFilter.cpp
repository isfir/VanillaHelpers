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

#include "MtxFilter.h"
#include "Common.h"
#include "Game.h"
#include "MinHook.h"
#include "Offsets.h"

#include <unordered_map>
#include <unordered_set>

namespace MtxFilter {

static Game::CGUnit_C_CreateUnitMount_t CGUnit_C_CreateUnitMount_o = nullptr;

static const std::unordered_set<uint32_t> g_storeMountIds = {
    18253, // Armored Brewfest Ram
    18436, // Tamed Rak'Shiri
    9991,  // Black Zulian Panther
    14329, // Armored Dawnsaber
    18435, // Azure Frostsaber
    9695,  // Ancient Frostsaber
    18255, // Striped Dawnsaber
    6443,  // Stranglethorn Tiger
    9714,  // Golden Leopard
    6442,  // Tawny Leopard
    4805,  // Spotted Leopard
    21108, // Corrupted Feral Raptor
    21106, // Red Feral Raptor
    21105, // Blue Feral Raptor
    20421, // Armored Ice Raptor
    14340, // Armored Obsidian Raptor
    14341, // Armored Red Raptor
    14343, // Armored Violet Raptor
    14345, // Armored Ivory Raptor
    21066, // Green Wind Rider
    21065, // Tawny Wind Rider
    21064, // Blue Wind Rider
    6478,  // Turquoise Tallstrider
    6475,  // Gray Tallstrider
    6474,  // Brown Tallstrider
    4807,  // Ivory Tallstrider
    18273, // Snowy Gryphon
    18272, // Ebon Gryphon
    18294, // Armored Ebon Gryphon
    18292, // Armored Snowy Gryphon
    18300, // Argent Hippogryph
    18298, // Cloudwing Hippogryph
    18299, // Cenarion Hippogryph
    18295, // Long-Forgotten Hippogryph
    18264, // Swift Blood Kodo
    2784,  // Ancient Black Ram
    21271, // Shadowhorn Stag
    20631, // Armored Brown Boar
    18817, // Cobalt Talbuk
    18816, // Dark Riding Talbuk
    18823, // Cobalt Armored Talbuk
    18822, // Black Armored Talbuk
    18090, // Darkmoon Dancing Bear
    20416, // Big Turtle WoW Bear
    18492, // Big Blizzard Bear
    18491, // Armored Frostmane Bear
    18484, // Armored Red Bear
    18488, // Armored Purple Bear
    18481, // Armored Black Bear
    18257, // Horde Worg
    2326,  // Ancient Red Wolf
    207,   // Ancient Black Wolf
    20425, // Brown Zhevra
    18243, // Zhevra
    21277, // Black Unicorn
    2185,  // Twilight Unicorn
    2186,  // White Unicorn
    20422, // Scarlet Charger
    20889, // Alliance Charger
    18260, // Armored Grey Steed
    18259, // Armored Black Steed
    18094, // Magic Rooster
    18180, // Raven Lord
    21100, // Cave Riding Spider
    18262, // Vermilion Deathcharger
    20804, // Black Deathcharger
    19170, // Twilight
    18143, // Azure Spectral Tiger
    20418, // Crimson Spectral Tiger
    20419, // Green Spectral Tiger
    20417, // Black Spectral Tiger
    20851, // Blazewing
    18523, // Happy Dalaran Cloud
    18524, // Dalaran Rain Cloud
    18510, // Turbo-Charged Flying Machine
    20651, // Celestial Steed
    20649, // Invincible
    18772, // Lovely Pink Furline
    21343, // Spectral Steed
};

static const std::unordered_map<uint32_t, std::vector<uint32_t>> g_racialMounts = {
    {1, {2404, 2405, 2408, 2409, 2410}}, // Human
    {2, {247, 2327, 2328}},              // Orc
    {3, {2736, 2785, 2786}},             // Dwarf
    {4, {6080, 6444, 6448}},             // Night Elf
    {5, {10670, 10671, 10672}},          // Undead
    {6, {11641, 12246}},                 // Tauren
    {8, {6569, 9473, 9475, 9476}},       // Gnome
    {9, {4806, 6472, 6473}},             // Troll
    {914, {18240}},                      // High Elf
    {927, {18319, 18320, 18321}},        // Goblin
};

static bool g_hideShopMounts = false;
static bool g_hideTurtleMount = false;

static void __fastcall CGUnit_C_CreateUnitMount_h(Game::CGUnit_C *unitptr) {
    const uint32_t originalDisplayId = unitptr->m_data->m_mountDisplayId;

    if ((g_hideShopMounts && g_storeMountIds.count(originalDisplayId)) ||
        (g_hideTurtleMount && originalDisplayId == 17158)) {
        const auto *factionTemplate = Game::DbLookupById<Game::FactionTemplate>(
            Game::g_factionTemplateDB, unitptr->m_data->m_factionTemplate);

        if (factionTemplate) {
            const auto it = g_racialMounts.find(factionTemplate->m_faction);
            if (it != g_racialMounts.end() && !it->second.empty()) {
                const auto &racialMounts = it->second;
                const uint32_t idx = unitptr->m_guid % racialMounts.size();
                unitptr->m_data->m_mountDisplayId = racialMounts[idx];
                CGUnit_C_CreateUnitMount_o(unitptr);
                unitptr->m_data->m_mountDisplayId = originalDisplayId;
                return;
            }
        }
    }

    CGUnit_C_CreateUnitMount_o(unitptr);
}

static bool __fastcall HideShopMountsObjectsCallback(void *context, void *edx, uint64_t guid) {
    auto *unitptr = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_UNIT, nullptr, guid, 0));
    if (unitptr == nullptr)
        return true;

    uint32_t mountDisplayId = unitptr->m_data->m_mountDisplayId;
    if (mountDisplayId > 0 && g_storeMountIds.count(mountDisplayId)) {
        Game::CGUnit_C_RefreshMount(unitptr, true);
    }

    return true;
}

static bool __fastcall HideTurtleMountObjectsCallback(void *context, void *edx, uint64_t guid) {
    auto *unitptr = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_UNIT, nullptr, guid, 0));
    if (unitptr == nullptr)
        return true;

    uint32_t mountDisplayId = unitptr->m_data->m_mountDisplayId;
    if (mountDisplayId == 17158) {
        Game::CGUnit_C_RefreshMount(unitptr, true);
    }

    return true;
}

static bool __fastcall HideShopMountsCVarCallback(void *CVar, const char *oldValue,
                                                  const char *newValue, void *userData) {
    if (!newValue || (newValue[0] != '0' && newValue[0] != '1') || newValue[1] != '\0')
        return false;

    g_hideShopMounts = newValue[0] == '1';

    if (oldValue && oldValue[0] != newValue[0] && Game::ClntObjMgrGetActivePlayer() > 0) {
        Game::ClntObjMgrEnumVisibleObjects(HideShopMountsObjectsCallback, nullptr);
    }

    return true;
}

static bool __fastcall HideTurtleMountCVarCallback(void *CVar, const char *oldValue,
                                                   const char *newValue, void *userData) {
    if (!newValue || (newValue[0] != '0' && newValue[0] != '1') || newValue[1] != '\0')
        return false;

    g_hideTurtleMount = newValue[0] == '1';

    if (oldValue && oldValue[0] != newValue[0] && Game::ClntObjMgrGetActivePlayer() > 0) {
        Game::ClntObjMgrEnumVisibleObjects(HideTurtleMountObjectsCallback, nullptr);
    }

    return true;
}

bool InstallHooks() {
    HOOK_FUNCTION(Offsets::FUN_CGUNIT_C_CREATE_UNIT_MOUNT, CGUnit_C_CreateUnitMount_h,
                  CGUnit_C_CreateUnitMount_o);

    return TRUE;
}
void Initialize() {
    Game::CVar_Register("VH_HideShopMounts", "Hide Shop Mounts", 1, "0", HideShopMountsCVarCallback,
                        Game::CVAR_CATEGORY::GRAPHICS, false, nullptr);
    Game::CVar_Register("VH_HideTurtleMount", "Hide Turtle Mount", 1, "0",
                        HideTurtleMountCVarCallback, Game::CVAR_CATEGORY::GRAPHICS, false, nullptr);
}

} // namespace MtxFilter
