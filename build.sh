mkdir build
cd build

cmake -G Ninja \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_INSTALL_RPATH=@rpath \
    ..
ninja foro-clang-format
