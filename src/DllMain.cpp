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

struct TrackedUnit {
    Game::HTEXTURE__ *texture;
    float scale;
};

static std::unordered_map<uint64_t, TrackedUnit> g_trackedUnitsBlip;
static std::unordered_map<uint64_t, Game::C2Vector> g_trackedUnitsMinimapPos;
static std::unordered_map<std::string, Game::HTEXTURE__ *> g_textureCache;

static bool IsValidFilename(const char *name) {
    return name && name[0] != '\0' && strpbrk(name, "<>:\"/\\|?*") == nullptr;
}

static int __cdecl Script_WriteFile(void *L) {
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

static int __cdecl Script_ReadFile(void *L) {
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

static int __cdecl Script_SetUnitBlip(void *L) {
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
        g_trackedUnitsMinimapPos[guid] = minimapPos;
    }
}

static void DrawTrackedBlips(Game::CGMinimapFrame *minimapPtr, Game::DNInfo *dnInfo) {
    // We are gathering the position in ObjectEnumProc because it is rate limited to avoid spamming
    // expensive calls. To do it in RenderObjectBlips, we can use DNInfo for current position,
    // MinimapGetWorldRadius() for world radius and minimapPtr +0x7C for layout scale.
    for (const auto &[guid, minimapPos] : g_trackedUnitsMinimapPos) {
        const auto it = g_trackedUnitsBlip.find(guid);
        if (it == g_trackedUnitsBlip.end())
            continue;
        const auto &trackedUnitBlip = it->second;

        Game::DrawMinimapTexture(trackedUnitBlip.texture, minimapPos, trackedUnitBlip.scale);
    }
}

void __stdcall LoadScriptFunctions_h() {
    LoadScriptFunctions_o();
    Game::RegisterLuaFunction("WriteFile", &Script_WriteFile, Offsets::CAVE_WRITE_FILE);
    Game::RegisterLuaFunction("ReadFile", &Script_ReadFile, Offsets::CAVE_READ_FILE);
    Game::RegisterLuaFunction("SetUnitBlip", &Script_SetUnitBlip, Offsets::CAVE_SET_UNIT_BLIPS);
}

int __fastcall ClntObjMgrEnumVisibleObjects_h(Game::ClntObjMgrEnumVisibleObjectsCallback_t callback,
                                              void *context) {
    if (reinterpret_cast<uintptr_t>(callback) == Offsets::FUN_OBJECT_ENUM_PROC) {
        g_trackedUnitsMinimapPos.clear();
    }
    return ClntObjMgrEnumVisibleObjects_o(callback, context);
}

int __fastcall ObjectEnumProc_h(Game::MINIMAPINFO *info, uint64_t guid) {
    // TODO: Check for tracked units here to avoid calling the original ObjectEnumProc
    // on units we're already tracking
    TrackUnit(info, guid);
    return ObjectEnumProc_o(info, guid);
}

void __fastcall RenderObjectBlips_h(Game::CGMinimapFrame *thisptr, void *edx,
                                    Game::DNInfo *dnInfo) {
    RenderObjectBlips_o(thisptr, dnInfo);
    DrawTrackedBlips(thisptr, dnInfo);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        if (MH_Initialize() != MH_OK)
            return FALSE;

        HOOK_FUNCTION(Offsets::FUN_LOAD_SCRIPT_FUNCTIONS, LoadScriptFunctions_h,
                      LoadScriptFunctions_o);
        HOOK_FUNCTION(Offsets::FUN_CLNT_OBJ_MGR_ENUM_VISIBLE_OBJECTS,
                      ClntObjMgrEnumVisibleObjects_h, ClntObjMgrEnumVisibleObjects_o);
        HOOK_FUNCTION(Offsets::FUN_OBJECT_ENUM_PROC, ObjectEnumProc_h, ObjectEnumProc_o);
        HOOK_FUNCTION(Offsets::FUN_RENDER_OBJECT_BLIP, RenderObjectBlips_h, RenderObjectBlips_o);

    } else if (reason == DLL_PROCESS_DETACH) {
        MH_Uninitialize();
    }
    return TRUE;
}
