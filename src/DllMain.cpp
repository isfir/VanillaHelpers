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

#include "Allocator.h"
#include "Blips.h"
#include "Common.h"
#include "FileIO.h"
#include "Game.h"
#include "MinHook.h"
#include "Morph.h"
#include "Offsets.h"
#include "Texture.h"

#include <string>

static Game::InitializeGlobal_t InitializeGlobal_o = nullptr;
static Game::FrameScript_Initialize_t FrameScript_Initialize_o = nullptr;
static Game::LoadScriptFunctions_t LoadScriptFunctions_o = nullptr;
static Game::CGGameUI_Shutdown_t CGGameUI_Shutdown_o = nullptr;

static void __fastcall InvalidFunctionPtrCheck_h() {}

static bool __fastcall InitializeGlobal_h() {
    bool ok = InitializeGlobal_o();
    Texture::InstallCharacterSkin();
    return ok;
}

static bool __fastcall FrameScript_Initialize_h() {
    FrameScript_Initialize_o();
    const std::string luaScript =
        "VANILLAHELPERS_VERSION=" + std::to_string(VANILLAHELPERS_VERSION_VALUE) +
        "\nVANILLA_HELPERS_VERSION=" + std::to_string(VANILLAHELPERS_VERSION_VALUE);
    Game::FrameScript_Execute(luaScript.c_str(), "VanillaHelpers.lua");
    return true;
}

static void __fastcall LoadScriptFunctions_h() {
    LoadScriptFunctions_o();
    FileIO::RegisterLuaFunctions();
    Blips::RegisterLuaFunctions();
    Morph::RegisterLuaFunctions();
}

static void __fastcall CGGameUI_Shutdown_h() {
    Morph::Reset();
    CGGameUI_Shutdown_o();
    Blips::Reset();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        Allocator::Initialize();
        Texture::Initialize();

        if (MH_Initialize() != MH_OK)
            return FALSE;

        auto *target = reinterpret_cast<LPVOID>(Offsets::FUN_INVALID_FUNCTION_PTR_CHECK);
        if (MH_CreateHook(target, static_cast<LPVOID>(InvalidFunctionPtrCheck_h), nullptr) != MH_OK)
            return FALSE;
        if (MH_EnableHook(target) != MH_OK)
            return FALSE;

        HOOK_FUNCTION(Offsets::FUN_INITIALIZE_GLOBAL, InitializeGlobal_h, InitializeGlobal_o);
        HOOK_FUNCTION(Offsets::FUN_FRAME_SCRIPT_INITIALIZE, FrameScript_Initialize_h,
                      FrameScript_Initialize_o);
        HOOK_FUNCTION(Offsets::FUN_LOAD_SCRIPT_FUNCTIONS, LoadScriptFunctions_h,
                      LoadScriptFunctions_o);
        HOOK_FUNCTION(Offsets::FUN_CGGAMEUI_SHUTDOWN, CGGameUI_Shutdown_h, CGGameUI_Shutdown_o);

        if (!Blips::InstallHooks())
            return FALSE;

        if (!Texture::InstallHooks())
            return FALSE;

        if (!Morph::InstallHooks())
            return FALSE;
    } else if (reason == DLL_PROCESS_DETACH) {
        MH_Uninitialize();
    }
    return TRUE;
}
