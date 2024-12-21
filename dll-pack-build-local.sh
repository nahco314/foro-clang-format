sh ./build.sh

mkdir ./artifacts/

LD_LIBRARY_PATH=$(pwd)/build/build/_deps/llvm_project-src/llvm/lib/ \
    dll-pack-builder local foro-clang-format \
    $(dll-pack-builder find ${BUILD_OUT_DIR}) \
    ./artifacts/ ${DLL_PACK_TARGET} ${GITHUB_REPOSITORY} ${GITHUB_REF#refs/tags/} \
    --include "$(pwd)/build/**" \
    --macho-rpath $(pwd)/build/build/_deps/llvm_project-src/llvm/lib/
