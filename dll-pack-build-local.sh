rustup target add ${DLL_PACK_TARGET}

cd ./crates/clang-binding/llvm-project

mkdir ./build
mkdir ./build/install
mkdir ./build/install/lib
cmake -S llvm -B build -G Ninja \
    -DCMAKE_INSTALL_PREFIX="./build/install" \
    -DLLVM_TARGETS_TO_BUILD= \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_PROJECTS='clang' \
    -DLLVM_INCLUDE_TESTS=0 \
    -DLLVM_INCLUDE_EXAMPLES=0 \
    -DLLVM_INCLUDE_BENCHMARKS=0 \
    -DLLVM_ENABLE_BINDINGS=0
78
cmake --build build --target clang-format
cmake --build build --target install

cd ../../../

cargo build --profile super-release --target ${DLL_PACK_TARGET}

mkdir ./artifacts/

LD_LIBRARY_PATH=./crates/clang-binding/llvm-project/build/install/lib \
    dll-pack-builder local $(cargo metadata --no-deps --format-version 1 | jq -r '.packages[0].name') \
    $(dll-pack-builder find ${BUILD_OUT_DIR}) \
    ./artifacts/ ${DLL_PACK_TARGET} ${GITHUB_REPOSITORY} ${GITHUB_REF#refs/tags/} \
    --include "./crates/clang-binding/llvm-project/build/install/lib/*"
