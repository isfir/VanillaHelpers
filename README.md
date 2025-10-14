# VanillaHelpers

This is a helper library for Vanilla WoW 1.12 that provides additional functionality.

## Features

- **File Operations**: Read/write files from the game's environment
- **Minimap Blips**: Customize unit markers on the minimap
- **Memory allocator**: Double the custom allocator's region size from 16 MiB to 32 MiB (128 regions), raising allocator capacity from 2 GiB to 4 GiB
- **High-Resolution Textures**: Increase the maximum supported texture size to 1024x1024 (from 512x512)
- **High-Resolution Character Skins**: Support up to 4x higher-resolution skin and armor textures

## Usage

The library registers these Lua functions:

```lua
WriteFile(filename, mode, content)
content = ReadFile(filename)
SetUnitBlip(unit [, texture [, scale]])
SetObjectTypeBlip(type [, texture [, scale]])
```

To enable higher-resolution character skins, add a text file at VanillaHelpers/ResizeCharacterSkin.txt inside your MPQ. The file should contain a single number: 2 or 4 (the scale multiplier).

See the source code for implementation details.

