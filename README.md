# Audio Player

OOP Assignment 2 under supervising of doctor Mohamed El-Rammly.

## Overview
This repository contains a C++ audio player project built as part of an Object-Oriented Programming (OOP) assignment.

## Requirements
- A C++ compiler supporting C++11 or newer (GCC, Clang, MSVC)
- (Optional) An IDE such as Visual Studio / CLion / Code::Blocks

## Build & Run
Build instructions depend on how the project is configured (CMake, Makefile, IDE project, etc.). Use one of the following approaches:

### Option A: Use the existing build system in the repo
1. Clone the repository:
   ```bash
   git clone https://github.com/am16ir/Audio_Player.git
   cd Audio_Player
   ```
2. If you see:
   - `CMakeLists.txt`: build with CMake
   - `Makefile`: run `make`
   - `*.sln`: open with Visual Studio

### Option B: Compile manually (example)
If the project is a small set of `.cpp` files without a build system:

```bash
git clone https://github.com/am16ir/Audio_Player.git
cd Audio_Player

g++ -std=c++11 -O2 -o audio_player *.cpp
./audio_player
```

## Project Structure (typical)
- `src/` — C++ source files
- `include/` — header files
- `main.cpp` — entry point

## Notes
If your project uses an external audio library (e.g., SFML, SDL2, PortAudio), install it and configure linking appropriately for your platform.

## License
No license file is currently provided. If you want others to reuse this project, consider adding a license (e.g., MIT).
