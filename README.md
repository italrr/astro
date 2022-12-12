astro ~> *
----------

* THIS PROJECT IS NO LONGER BEING DEVELOPED. SEE https://github.com/italrr/cavernrush*

astro is a highly concurrent 3D game engine made in C++11. It's targeted to support Vulkan and OpenGL 4.5 out of the box. 

### Objectives & philosophies applied since day one:
- High concurrency
- Major preference for parallel pipelines
- Open World esque world maps and scenes
- Platform agnostic rendering engine out-of-the-box, aiming to support the following for release version:
    - OpenGL
    - Vulkan
- Multi-platform/OS support:
    - Linux
    - Windows
- Ability to modify any aspects of the engine through scripts and JSON files
- Easily moddable games
- Native scripting language built-in (Lua)

### Dependencies
Note: some of the math code used in this software was taken from GLM. I will include apropiate disclaimers in the future.

- Assimp
- FreeType

### How to build
Basically: 
```
cmake .
make
./astro
```

### About the game
The game I've decided to make with this engine is a co-op procedurally generated dungeon-crawler. Major classic RPG characteristics will be a must in the design.

### WIP [ENGINE] [P -> Progress | D -> Done | S -> Stalled]
#### Stage 1 "Get it to work"
- [D] Implement Assimp for 3D model loading
- [D] 3D model animation
- [D] Texture loading
- [D] Shader loading
- [D] Basic camera
- [D] File management system (indexing, asset loader, etc)
- [D] Input (Keyboard/Mouse)
- [P] Sync/async job system
- [P] Basic general utilities (File manipulation/reading, hashing, string manipulation, time, etc)
- [P] Math utilities (matrix and vector operations, etc.)
- [P] Basic lighting system
- [P] Multi-threading architecture (Rendering thread, engine core thread, game thread, etc.)
- [P] OpenGL support
- [S] Network Support
- [S] Data/file transfer
- [S] Font loading/rendering
- [S] UI System
- [S] Multi-layer rendering (GUI, Console, 3D world, Menues)
- [S] World map (streaming) system
- [S] World simulation
- [S] In-game console
- [S] Basic 3D Physics
- [S] Basic 3D audio
- [S] Integrate Lua

#### Stage 2 "Make it fancy"
- [] Advanced lighting system

#### Stage 3 "Ok, now we're talking"
- [] Vulkan Support
- [] Gamepad support

