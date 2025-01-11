#include <libsm64_mario_geometry.hpp>

LibSM64MarioGeometry::LibSM64MarioGeometry()
{
    geometry.position = position.data();
    geometry.normal = normal.data();
    geometry.color = color.data();
    geometry.uv = uv.data();

    geometry.numTrianglesUsed = 0;
}
