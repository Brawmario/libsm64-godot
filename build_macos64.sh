#!/bin/bash

PROJECT_ROOT=$PWD

GODOT_INC="-I${PROJECT_ROOT}/godot-cpp/include -I${PROJECT_ROOT}/godot-cpp/include/core -I${PROJECT_ROOT}/godot-cpp/include/gen -I${PROJECT_ROOT}/godot-cpp/godot-headers"
GODOT_LIB="-L${PROJECT_ROOT}/godot-cpp/bin -lgodot-cpp.osx.release.64"
LIBSM64_INC="-I${PROJECT_ROOT}/libsm64/dist/include"
LIBSM64_LIB="-Lbin/macos64 -lsm64"

git submodule update --init --recursive

cd godot-cpp
scons platform=osx generate_bindings=yes target=release -j4
cd ..

cd libsm64
make -j4
cd ..

cd addons/libsm64/sm64_handler

mkdir -p bin/macos64/

cp -f ${PROJECT_ROOT}/libsm64/dist/libsm64.so bin/macos64/

c++ -fPIC -o src/init.o        -c src/init.cpp        -g -O3 -std=c++14 -Isrc ${GODOT_INC} ${LIBSM64_INC}
c++ -fPIC -o src/SM64Handler.o -c src/SM64Handler.cpp -g -O3 -std=c++14 -Isrc ${GODOT_INC} ${LIBSM64_INC}

c++ -o bin/macos64/libsm64godot.so -shared src/init.o src/SM64Handler.o ${GODOT_LIB} ${LIBSM64_LIB}