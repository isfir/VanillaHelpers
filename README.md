# VanillaHelpers

This is a helper library for Vanilla WoW 1.12 that provides additional functionality.

## Features

- **File Operations**: Read/write files from the game's environment
- **Minimap Blips**: Customize unit markers on the minimap

## Usage

The library registers these Lua functions:

```lua
WriteFile(filename, mode, content)
SetUnitBlip(unit [, texture [, scale]])
```

See the source code for implementation details.

