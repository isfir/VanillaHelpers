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

#include "Blips.h"
#include "FileIO.h"
#include "Game.h"
#include "MinHook.h"
#include "Offsets.h"

static Game::LoadScriptFunctions_t LoadScriptFunctions_o = nullptr;

static void __fastcall InvalidFunctionPtrCheck_h() {}

static void __fastcall LoadScriptFunctions_h() {
    LoadScriptFunctions_o();
    FileIO::RegisterLuaFunctions();
    Blips::RegisterLuaFunctions();
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

        target = reinterpret_cast<LPVOID>(Offsets ::FUN_LOAD_SCRIPT_FUNCTIONS);
        if (MH_CreateHook(target, static_cast<LPVOID>(LoadScriptFunctions_h), reinterpret_cast<LPVOID *>(&LoadScriptFunctions_o)))
            return FALSE;
        if (MH_EnableHook(target) != MH_OK)
            return FALSE;

        if (!Blips::InstallHooks())
            return FALSE;
    } else if (reason == DLL_PROCESS_DETACH) {
        MH_Uninitialize();
    }
    return TRUE;
}
