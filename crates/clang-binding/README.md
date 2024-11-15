```
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
    -DLLVM_ENABLE_BINDINGS=0 \
    -DLLVM_USE_LINKER=mold \
    -DLLVM_PARALLEL_COMPILE_JOBS=20 \
    -DLLVM_PARALLEL_LINK_JOBS=20 \
    -DLLVM_PARALLEL_TABLEGEN_JOBS=20

cmake --build build --target clang-format
sudo cmake --build build --target install
```

```
mkdir ./build
mkdir ./build/wasm
mkdir ./build/wasm/install
mkdir ./build/wasm/install/lib

cmake -S llvm -B build -G Ninja \
    -DCMAKE_INSTALL_PREFIX="./build/wasm/install" \
    -DLLVM_ENABLE_PROJECTS='clang' \
    -DLLVM_TARGETS_TO_BUILD=WebAssembly \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_RUNTIME_TARGETS=wasm32-wasip1 \
    -DCMAKE_SYSTEM_NAME=Generic \
    -DLLVM_HOST_TRIPLE=wasm32-wasip1 \
    -DLLVM_INCLUDE_TESTS=0 \
    -DLLVM_INCLUDE_EXAMPLES=0 \
    -DLLVM_INCLUDE_BENCHMARKS=0 \
    -DLLVM_ENABLE_BINDINGS=0 \
    -DLLVM_PARALLEL_COMPILE_JOBS=20 \
    -DLLVM_PARALLEL_LINK_JOBS=20 \
    -DLLVM_PARALLEL_TABLEGEN_JOBS=20

cmake --build build --target clang-format
sudo cmake --build build --target install

cd ../

curl -L https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-24/wasi-sysroot-24.0.tar.gz | tar xz
```
