# Universe-Simulation
Fast CPU-based particle simulation

## Requirements (Windows)

Before building the project, install:

- Visual Studio 2022 with the **Desktop development with C++** workload
  **or**
- MSYS2 with:
  - GCC (MinGW-w64)
  - CMake
  - Ninja (or Make)
  - Git

If the project uses SFML, install the appropriate SFML package compatible with your compiler.

### Required tools

- C++20 compatible compiler
- CMake 3.20 or newer
- Git
- Ninja or Make (recommended)

### MSYS2 packages

```sh
pacman -S --needed \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-ninja \
    git
```
## Requirements (Linux)

Before building the project, install:

- GCC or Clang with C++20 support
- CMake 3.20 or newer
- Git
- Ninja or Make

### Ubuntu / Linux Mint

```sh
sudo apt update
sudo apt install build-essential cmake ninja-build git
```

If the project uses additional libraries (such as SFML), install them before building.
