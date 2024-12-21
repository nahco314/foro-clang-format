sh ./build.sh

mkdir ./artifacts/

dll-pack-builder local foro-clang-format \
    $(dll-pack-builder find ${BUILD_OUT_DIR}) \
    ./artifacts/ ${DLL_PACK_TARGET} ${GITHUB_REPOSITORY} ${GITHUB_REF#refs/tags/}
