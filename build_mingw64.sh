#!/bin/bash

cp -u /mingw64/bin/libgcc_s_seh-1.dll  bin/
cp -u /mingw64/bin/libstdc++-6.dll     bin/
cp -u /mingw64/bin/libwinpthread-1.dll bin/
cp -u libsm64/dist/sm64.dll            bin/

g++ -fPIC -o src/init.o -c src/init.cpp -g -O3 -std=c++14 -Isrc -Igodot-cpp/include -Igodot-cpp/include/core -Igodot-cpp/include/gen -Igodot-cpp/godot-headers -Ilibsm64/dist/include
g++ -fPIC -o src/SM64Handler.o -c src/SM64Handler.cpp -g -O3 -std=c++14 -Isrc -Igodot-cpp/include -Igodot-cpp/include/core -Igodot-cpp/include/gen -Igodot-cpp/godot-headers -Ilibsm64/dist/include
g++ -o bin/libsm64godot.dll -shared src/init.o src/SM64Handler.o -Lgodot-cpp/bin -lgodot-cpp.windows.debug.64 -Lbin -lsm64