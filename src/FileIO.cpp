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

#include "FileIO.h"
#include "Game.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

namespace FileIO {

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

void RegisterLuaFunctions() {
    Game::FrameScript_RegisterFunction("WriteFile", reinterpret_cast<uintptr_t>(&Script_WriteFile));
    Game::FrameScript_RegisterFunction("ReadFile", reinterpret_cast<uintptr_t>(&Script_ReadFile));
}

} // namespace FileIO
