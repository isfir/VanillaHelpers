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

#include "Game.h"
#include "MinHook.h"
#include "Offsets.h"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>

#define HOOK_FUNCTION(offset, hook, original)                                                      \
    {                                                                                              \
        auto *target = reinterpret_cast<LPVOID>(offset);                                           \
        if (MH_CreateHook(target, static_cast<LPVOID>(hook),                                       \
                          reinterpret_cast<LPVOID *>(&(original))) != MH_OK)                       \
            return FALSE;                                                                          \
        if (MH_EnableHook(target) != MH_OK)                                                        \
            return FALSE;                                                                          \
    }

static Game::LoadScriptFunctions_t LoadScriptFunctions_o = nullptr;
static Game::RenderObjectBlips_t RenderObjectBlips_o = nullptr;
static Game::ObjectEnumProc_t ObjectEnumProc_o = nullptr;
static Game::ClntObjMgrEnumVisibleObjects_t ClntObjMgrEnumVisibleObjects_o = nullptr;
static void *OnLayerTrackUpdate_ChangedGate_o = nullptr;
static void *OnLayerTrackUpdate_PreShowGate_o = nullptr;
static void *OnLayerTrackUpdate_AppendToTooltipBuffer_o = nullptr;

struct TrackedUnitBlip {
    Game::HTEXTURE__ *texture;
    float scale;
};

struct TrackedUnitData {
    Game::C2Vector minimapPos;
    bool isInDifferentArea;
    std::string name;
};

struct BlipHoverEntry {
    uint64_t guid;
    std::string name;
    bool gray;
};

struct BlipHoverState {
    bool changed = false;
    bool nonEmpty = false;
    uint64_t hash = 0;
    std::vector<BlipHoverEntry> hits;
};

static std::unordered_map<std::string, Game::HTEXTURE__ *> g_textureCache;
static std::unordered_map<uint64_t, TrackedUnitBlip> g_trackedUnitsBlip;
static std::unordered_map<uint64_t, TrackedUnitData> g_trackedUnitsData;
static BlipHoverState g_blipHoverState;

static bool IsValidFilename(const char *name) {
    return name && name[0] != '\0' && strpbrk(name, "<>:\"/\\|?*") == nullptr;
}

static int __fastcall Script_WriteFile(void *L) {
    if (!Game::Lua::IsString(L, 1) || !Game::Lua::IsString(L, 2) || !Game::Lua::IsString(L, 3)) {
        Game::Lua::Error(L, "Usage: WriteFile(filename, mode, content)");
        return 0;
    }

    const auto *filename = Game::Lua::ToString(L, 1);
    const auto *mode = Game::Lua::ToString(L, 2);
    const auto *content = Game::Lua::ToString(L, 3);

    if (!IsValidFilename(filename)) {
        Game::Lua::Error(L, "Invalid or empty filename (must not contain: < > : \" / \\ | ? *)");
        return 0;
    }
    if (strcmp(mode, "w") != 0 && strcmp(mode, "a") != 0) {
        Game::Lua::Error(L, "Invalid mode: must be either 'w' or 'a'");
        return 0;
    }

    std::error_code ec;
    std::filesystem::create_directories("VanillaHelpersData", ec);

    auto fullPath = std::filesystem::path("VanillaHelpersData") / filename;

    auto openmode = std::ios::out;
    openmode |= (mode[0] == 'w') ? std::ios::trunc : std::ios::app;

    std::ofstream out(fullPath, openmode);
    if (!out) {
        Game::Lua::Error(L, "Failed to open file for writing.");
        return 0;
    }

    out << content;
    if (!out) {
        Game::Lua::Error(L, "Failed to write file.");
        return 0;
    }

    return 0;
}

static int __fastcall Script_ReadFile(void *L) {
    if (!Game::Lua::IsString(L, 1)) {
        Game::Lua::Error(L, "Usage: ReadFile(filename)");
        return 0;
    }

    const auto *filename = Game::Lua::ToString(L, 1);
    if (!IsValidFilename(filename)) {
        Game::Lua::Error(L, "Invalid or empty filename (must not contain: < > : \" / \\ | ?*)");
        return 0;
    }

    auto fullPath = std::filesystem::path("VanillaHelpersData") / filename;

    std::ifstream in(fullPath);
    if (!in) {
        Game::Lua::Error(L, "Failed to open file for reading.");
        return 0;
    }

    std::ostringstream buf;
    buf << in.rdbuf();
    std::string data = buf.str();

    Game::Lua::PushString(L, data.c_str());
    return 1;
}

static int __fastcall Script_SetUnitBlip(void *L) {
    if (!Game::Lua::IsString(L, 1)) {
        Game::Lua::Error(L, "Usage: SetUnitBlip(unit [, texture [, scale]])");
        return 0;
    }

    const auto *unitName = Game::Lua::ToString(L, 1);
    uint64_t unitGUID = Game::GetGUIDFromName(unitName);
    Game::CGObject_C *unitptr =
        Game::ClntObjMgrObjectPtr(Game::TYPEMASK_UNIT, nullptr, unitGUID, 0);

    // TODO: Add UnitCanAssist check
    if (unitptr == nullptr) {
        Game::Lua::Error(L, "Unit not found.");
        return 0;
    }

    if (!Game::Lua::IsString(L, 2)) {
        g_trackedUnitsBlip.erase(unitGUID);
        return 0;
    }

    // TODO: Path should be lower cased to avoid duplicates
    const auto *texturePath = Game::Lua::ToString(L, 2);

    Game::HTEXTURE__ *texture = nullptr;

    const auto it = g_textureCache.find(texturePath);
    if (it == g_textureCache.end()) {
        Game::CGxTexFlags texFlags(Game::EGxTexFilter::GxTex_Nearest, 0, 0, 0, 0, 0, 0, 1);
        Game::CStatus status;
        texture = Game::TextureCreate(texturePath, &status, texFlags, 0, 1);

        if (!status.ok()) {
            Game::Lua::Error(L, "Couldn't load the texture.");
            return 0;
        }

        g_textureCache[texturePath] = texture;
    } else {
        texture = it->second;
    }

    float scale = 1;

    if (Game::Lua::IsNumber(L, 3)) {
        scale = static_cast<float>(Game::Lua::ToNumber(L, 3));
    }

    g_trackedUnitsBlip[unitGUID] = {texture, scale};

    return 0;
}

static void TrackUnit(Game::MINIMAPINFO *info, uint64_t guid) {
    const auto it = g_trackedUnitsBlip.find(guid);
    if (it == g_trackedUnitsBlip.end())
        return;

    Game::CGObject_C *unitptr = Game::ClntObjMgrObjectPtr(Game::TYPEMASK_UNIT, nullptr, guid, 0);

    if (unitptr != nullptr) {
        Game::C2Vector minimapPos = {0, 0};

        Game::C3Vector unitPos{};
        unitptr->vftable->GetPosition(unitptr, &unitPos);

        float unkScale = info->minimapFrame->FrameScriptPart.vftable->GetUnkScale(
            &info->minimapFrame->FrameScriptPart);

        Game::WorldPosToMinimapFrameCoords(&minimapPos, nullptr, info->currentPos, info->radius,
                                           unitPos.x, unitPos.y, info->layoutScale, unkScale);

        // TODO: Add isInDifferentArea info to support graying blip, this can be obtained by
        // comparing info->unk1 (something like player area ID) and the output of the function at
        // 0x670540
        g_trackedUnitsData[guid] = {minimapPos, false, unitptr->vftable->GetName(unitptr)};
    }
}

static void DrawTrackedBlips(Game::CGMinimapFrame *minimapPtr, Game::DNInfo *dnInfo) {
    // We are gathering the position in ObjectEnumProc because it is rate limited to avoid spamming
    // expensive calls. To do it in RenderObjectBlips, we can use DNInfo for current position,
    // MinimapGetWorldRadius() for world radius and minimapPtr +0x7C for layout scale.
    for (const auto &[guid, trackedUnitData] : g_trackedUnitsData) {
        const auto it = g_trackedUnitsBlip.find(guid);
        if (it == g_trackedUnitsBlip.end())
            continue;
        const auto &trackedUnitBlip = it->second;

        // TODO: Check how the original function is putting unit name on blips when mouseovering
        // them
        Game::DrawMinimapTexture(trackedUnitBlip.texture, trackedUnitData.minimapPos,
                                 trackedUnitBlip.scale);
    }
}

static void UpdateCustomHover(Game::C2Vector mouse, Game::C2Vector offset) {
    std::vector<BlipHoverEntry> now;
    now.reserve(g_trackedUnitsData.size());

    for (const auto &[guid, unit] : g_trackedUnitsData) {
        const auto it = g_trackedUnitsBlip.find(guid);
        if (it == g_trackedUnitsBlip.end())
            continue;
        const auto &bl = it->second;

        const float half = Game::BLIP_HALF * bl.scale;
        const float px = unit.minimapPos.x + offset.x;
        const float py = unit.minimapPos.y + offset.y;

        if (fabsf(mouse.x - px) <= half && fabsf(mouse.y - py) <= half) {
            now.push_back({guid, unit.name, unit.isInDifferentArea});
        }
    }

    std::sort(now.begin(), now.end(), [](const auto &a, const auto &b) { return a.guid < b.guid; });

    // FNV-1a hash
    uint64_t h = 14695981039346656037ULL;
    auto mix = [&](uint64_t v) {
        h ^= v;
        h *= 1099511628211ULL;
    };
    for (const auto &hit : now) {
        mix(hit.guid);
        mix(hit.gray ? 1ULL : 0ULL);
        mix(std::hash<std::string_view>{}(hit.name));
    }

    g_blipHoverState.nonEmpty = !now.empty();
    const bool changed = (h != g_blipHoverState.hash);
    g_blipHoverState.changed = changed;
    if (changed) {
        g_blipHoverState.hash = h;
        g_blipHoverState.hits.swap(now);
    }
}

static void WriteToMinimapTooltip(char *tooltipText) {
    if (g_blipHoverState.hits.empty())
        return;

    for (const auto &hit : g_blipHoverState.hits) {
        if (hit.gray) {
            Game::SStrPack(tooltipText, "|cffb0b0b0", 0x400);
            Game::SStrPack(tooltipText, hit.name.c_str(), 0x400);
            Game::SStrPack(tooltipText, "|r\n", 0x400);
        } else {
            Game::SStrPack(tooltipText, hit.name.c_str(), 0x400);
            Game::SStrPack(tooltipText, "\n", 0x400);
        }
    }
}

static void __fastcall InvalidFunctionPtrCheck_h() {}

static void __fastcall LoadScriptFunctions_h() {
    LoadScriptFunctions_o();
    Game::FrameScript_RegisterFunction("WriteFile", reinterpret_cast<uintptr_t>(&Script_WriteFile));
    Game::FrameScript_RegisterFunction("ReadFile", reinterpret_cast<uintptr_t>(&Script_ReadFile));
    Game::FrameScript_RegisterFunction("SetUnitBlip",
                                       reinterpret_cast<uintptr_t>(&Script_SetUnitBlip));
}

static int __fastcall ClntObjMgrEnumVisibleObjects_h(
    Game::ClntObjMgrEnumVisibleObjectsCallback_t callback, void *context) {
    if (reinterpret_cast<uintptr_t>(callback) == Offsets::FUN_OBJECT_ENUM_PROC) {
        g_trackedUnitsData.clear();
    }
    return ClntObjMgrEnumVisibleObjects_o(callback, context);
}

static int __fastcall ObjectEnumProc_h(Game::MINIMAPINFO *info, uint64_t guid) {
    // TODO: Check for tracked units here to avoid calling the original ObjectEnumProc
    // on units we're already tracking
    TrackUnit(info, guid);
    return ObjectEnumProc_o(info, guid);
}

static void __fastcall RenderObjectBlips_h(Game::CGMinimapFrame *thisptr, void * /*edx*/,
                                           Game::DNInfo *dnInfo) {
    RenderObjectBlips_o(thisptr, dnInfo);
    DrawTrackedBlips(thisptr, dnInfo);
}

// Right before the early-out check uses ECX to decide if anything changed
static void __declspec(naked) OnLayerTrackUpdate_ChangedGate_h() {
    __asm {
        pushad

        mov  eax, [ebx+0x8] // param pointer
        push dword ptr [ebp-0x40] // offsetY
        push dword ptr [ebp-0x3C] // offsetX
        push dword ptr [eax+0x28] // mouseY
        push dword ptr [eax+0x24] // mouseX
        call UpdateCustomHover
        add  esp, 16

        popad

            // If our set changed, force ECX=1 (stock "changed" flag)
        cmp  byte ptr [g_blipHoverState.changed], 0
        je   short no_custom_change
        mov  ecx, 1
    no_custom_change:

        jmp  dword ptr [OnLayerTrackUpdate_ChangedGate_o]
    }
}

// Right before the branch that decides whether to build/update the tooltip text
static void __declspec(naked) OnLayerTrackUpdate_PreShowGate_h() {
    __asm {
        // If we have any changes, force EDX=1 so the stock code builds the tooltip
        cmp  byte ptr [g_blipHoverState.changed], 0
        je   short no_set_edx
        mov  edx, 1
    no_set_edx:

        jmp  dword ptr [OnLayerTrackUpdate_PreShowGate_o]
    }
}

// After the scratch tooltip buffer is initialized, before original blips text are appended
static void __declspec(naked) OnLayerTrackUpdate_AppendToTooltipBuffer_h() {
    __asm {
        // Append into stock scratch buffer [EBP-0x468]
        pushad
        lea  eax, [ebp-0x468]
        push eax
        call WriteToMinimapTooltip
        add  esp, 4
        popad

        jmp  dword ptr [OnLayerTrackUpdate_AppendToTooltipBuffer_o]
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        if (MH_Initialize() != MH_OK)
            return FALSE;

        auto *target = reinterpret_cast<LPVOID>(Offsets::FUN_INVALID_FUNCTION_PTR_CHECK);
        if (MH_CreateHook(target, static_cast<LPVOID>(InvalidFunctionPtrCheck_h), nullptr) != MH_OK)
            return FALSE;
        if (MH_EnableHook(target) != MH_OK)
            return FALSE;

        HOOK_FUNCTION(Offsets::FUN_LOAD_SCRIPT_FUNCTIONS, LoadScriptFunctions_h,
                      LoadScriptFunctions_o);
        HOOK_FUNCTION(Offsets::FUN_CLNT_OBJ_MGR_ENUM_VISIBLE_OBJECTS,
                      ClntObjMgrEnumVisibleObjects_h, ClntObjMgrEnumVisibleObjects_o);
        HOOK_FUNCTION(Offsets::FUN_OBJECT_ENUM_PROC, ObjectEnumProc_h, ObjectEnumProc_o);
        HOOK_FUNCTION(Offsets::FUN_RENDER_OBJECT_BLIP, RenderObjectBlips_h, RenderObjectBlips_o);
        HOOK_FUNCTION(Offsets::PATCH_MINIMAP_TRACK_UPDATE_CHANGED_GATE,
                      OnLayerTrackUpdate_ChangedGate_h, OnLayerTrackUpdate_ChangedGate_o);
        HOOK_FUNCTION(Offsets::PATCH_MINIMAP_TRACK_UPDATE_PRE_SHOW_GATE,
                      OnLayerTrackUpdate_PreShowGate_h, OnLayerTrackUpdate_PreShowGate_o);
        HOOK_FUNCTION(Offsets::PATCH_MINIMAP_TRACK_UPDATE_APPEND_TO_TOOLTIP_BUFFER,
                      OnLayerTrackUpdate_AppendToTooltipBuffer_h,
                      OnLayerTrackUpdate_AppendToTooltipBuffer_o);

    } else if (reason == DLL_PROCESS_DETACH) {
        MH_Uninitialize();
    }
    return TRUE;
}
