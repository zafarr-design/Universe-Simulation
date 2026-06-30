echo setup...
set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo builing...
mkdir -p "$PROJECT_DIR/build-linux" && cd "$PROJECT_DIR/build-linux"
cmake "$PROJECT_DIR" -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
echo done!
cd ..

