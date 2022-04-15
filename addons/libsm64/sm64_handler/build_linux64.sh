#!/bin/bash

PROJECT_ROOT=../../..

GODOT_INC="-I${PROJECT_ROOT}/godot-cpp/include -I${PROJECT_ROOT}/godot-cpp/include/core -I${PROJECT_ROOT}/godot-cpp/include/gen -I${PROJECT_ROOT}/godot-cpp/godot-headers"
GODOT_LIB="-L${PROJECT_ROOT}/godot-cpp/bin -lgodot-cpp.linux.release.64"
LIBSM64_INC="-I${PROJECT_ROOT}/libsm64/dist/include"
LIBSM64_LIB="-Lbin/linux64 -lsm64"

mkdir -p bin/linux64/

cp -u ${PROJECT_ROOT}/libsm64/dist/libsm64.so bin/linux64/

g++ -fPIC -o src/init.o        -c src/init.cpp        -g -O3 -std=c++14 -Isrc ${GODOT_INC} ${LIBSM64_INC}
g++ -fPIC -o src/SM64Handler.o -c src/SM64Handler.cpp -g -O3 -std=c++14 -Isrc ${GODOT_INC} ${LIBSM64_INC}

g++ -o bin/linux64/libsm64godot.so -shared src/init.o src/SM64Handler.o ${GODOT_LIB} ${LIBSM64_LIB}