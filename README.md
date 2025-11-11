# VanillaHelpers

This is a helper library for Vanilla WoW 1.12 that provides additional functionality.

## Installation

It is recommended to use [VanillaFixes](https://github.com/hannesmann/vanillafixes) to load this mod.

**Steps:**

1. Install `VanillaFixes` if not already installed
2. Copy `VanillaHelpers.dll` to your game directory
3. Add `VanillaHelpers.dll` to the `dlls.txt` file
4. Start the game with `VanillaFixes.exe`

**Note:** While it might be possible to use a popular private server client modification to load the DLL, this method is not recommended. The way that client loads DLLs is inferior and may introduce bugs. VanillaFixes loads the DLL before any game code is run, which is more reliable. Some bugs that occurred when loaded by the other method have been fixed, but there is no guarantee that all issues will be addressed, especially if they are not easy to resolve.

## Features

- **File Operations**: Read/write files from the game's environment
- **Minimap Blips**: Customize unit markers on the minimap
- **Memory Allocator**: Double the custom allocator's region size from 16 MiB to 32 MiB (128 regions), raising allocator capacity from 2 GiB to 4 GiB
- **High-Resolution Textures**: Increase the maximum supported texture size to 1024x1024 (from 512x512)
- **High-Resolution Character Skins**: Support up to 4x higher-resolution skin and armor textures
- **Character Morph**: Change character appearances, mounts, and visible items

## Usage

The library registers these Lua functions:

```lua
WriteFile(filename, mode, content)
content = ReadFile(filename)
SetUnitBlip(unit [, texture [, scale]])
SetObjectTypeBlip(type [, texture [, scale]])
SetUnitDisplayID(unitToken [, displayID])
RemapDisplayID(oldDisplayID(s) [, newDisplayID])
SetUnitMountDisplayID(unitToken [, mountDisplayID])
RemapMountDisplayID(oldDisplayID(s) [, factionIndexedDisplayIDs])
SetUnitVisibleItemID(unitToken, inventorySlot [, displayID])
RemapVisibleItemID(oldDisplayID(s), inventorySlot [, newDisplayID])
displayID, nativeDisplayID, mountDisplayID = UnitDisplayInfo(unitToken)
itemDisplayID = GetItemDisplayID(itemID)
```

To enable higher-resolution character skins, add a text file at VanillaHelpers/ResizeCharacterSkin.txt inside your MPQ. The file should contain a single number: 2 or 4 (the scale multiplier).

See the source code for implementation details.

## Memory Usage Note

When using high-resolution textures, particularly 4x character skins, the game will use significantly more RAM and might run out of memory in crowded areas. To help avoid this, it is recommended to run the game on a 64-bit operating system and use DXVK. Additionally, ensure the game executable is Large Address Aware (LAA) flagged to access more than 2GB of RAM (note that the popular private server client already has this flag set).

If running on Linux, it's recommended to use a Wine build with WoW64 mode and apply [this patch](https://bugs.winehq.org/attachment.cgi?id=75848) to make more addresses available in the 32-bit address space.

