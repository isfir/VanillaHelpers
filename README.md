# VanillaHelpers

This is a helper library for Vanilla WoW 1.12 that provides additional functionality.

## Features

- **File Operations**: Read/write files from the game's environment
- **Minimap Blips**: Customize unit markers on the minimap
- **Memory allocator**: Double the custom allocator's region size from 16 MiB to 32 MiB (128 regions), raising allocator capacity from 2 GiB to 4 GiB

## Usage

The library registers these Lua functions:

```lua
WriteFile(filename, mode, content)
content = ReadFile(filename)
SetUnitBlip(unit [, texture [, scale]])
SetObjectTypeBlip(type [, texture [, scale]])
```

See the source code for implementation details.

