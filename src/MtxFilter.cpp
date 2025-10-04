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

#include <unordered_set>

namespace MtxFilter {

static const std::unordered_set<uint32_t> storeMountIds = {
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
};

static Game::CGUnit_C_CreateUnitMount_t CGUnit_C_CreateUnitMount_o = nullptr;

static bool g_hideShopMounts = false;

static void __fastcall CGUnit_C_CreateUnitMount_h(Game::CGUnit_C *unitptr) {
    uint32_t mountDisplayId = unitptr->m_data->m_mountDisplayId;
    if (g_hideShopMounts && storeMountIds.count(mountDisplayId)) {
        const auto *factionTemplate = Game::DbLookupById<Game::FactionTemplate>(
            Game::g_factionTemplateDB, unitptr->m_data->m_factionTemplate);
        unitptr->m_data->m_mountDisplayId =
            factionTemplate && factionTemplate->m_factionGroup & 0x4 ? 247 : 2404;
        CGUnit_C_CreateUnitMount_o(unitptr);
        unitptr->m_data->m_mountDisplayId = mountDisplayId;
    } else {
        CGUnit_C_CreateUnitMount_o(unitptr);
    }
}

static bool __fastcall VisibleObjectsMountsCallback(void *context, void *edx, uint64_t guid) {
    auto *unitptr = reinterpret_cast<Game::CGUnit_C *>(
        Game::ClntObjMgrObjectPtr(Game::TYPE_MASK::TYPEMASK_UNIT, nullptr, guid, 0));
    if (unitptr == nullptr)
        return true;

    uint32_t mountDisplayId = unitptr->m_data->m_mountDisplayId;
    if (mountDisplayId > 0 && storeMountIds.count(mountDisplayId)) {
        Game::CGUnit_C_RefreshMount(unitptr, true);
    }

    return true;
}

static bool __fastcall HideShopMountsCallback(void *CVar, const char *oldValue,
                                              const char *newValue, void *userData) {
    if (!newValue || (newValue[0] != '0' && newValue[0] != '1') || newValue[1] != '\0')
        return false;
    g_hideShopMounts = newValue[0] == '1';
    if (oldValue && oldValue[0] != newValue[0]) {
        Game::ClntObjMgrEnumVisibleObjects(VisibleObjectsMountsCallback, nullptr);
    }
    return true;
}

bool InstallHooks() {
    HOOK_FUNCTION(Offsets::FUN_CGUNIT_C_CREATE_UNIT_MOUNT, CGUnit_C_CreateUnitMount_h,
                  CGUnit_C_CreateUnitMount_o);

    return TRUE;
}
void Initialize() {
    Game::CVar_Register("VH_HideShopMounts", "Hide Shop Mounts", 1, "0", HideShopMountsCallback,
                        Game::CVAR_CATEGORY::GRAPHICS, false, nullptr);
}

} // namespace MtxFilter
