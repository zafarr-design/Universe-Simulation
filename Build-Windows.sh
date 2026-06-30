#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo ">>> Windows..."
mkdir -p "$PROJECT_DIR/build-win" && cd "$PROJECT_DIR/build-win"
cmake "$PROJECT_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$PROJECT_DIR/toolchain-mingw64.cmake" \
    -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) 
cd ..

echo ">>> Copying MinGW runtime DLLs..."

MINGW_BIN="/usr/x86_64-w64-mingw32/sys-root/mingw/bin"

cp /usr/lib/gcc/x86_64-w64-mingw32/13-posix/libstdc++-6.dll  ./Game
cp /usr/lib/gcc/x86_64-w64-mingw32/13-posix/libgcc_s_seh-1.dll ./Game
cp /usr/lib/gcc/x86_64-w64-mingw32/13-posix/libgomp-1.dll ./Game/
cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll ./Game

echo ">>> Gotowe!"
echo "Linux:   $PROJECT_DIR/Holy.Rock"
echo "Windows: $PROJECT_DIR/Holy.Rock.exe"
