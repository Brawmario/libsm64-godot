#include <sm64_mario_geometry.hpp>

#include <string.h>

SM64MarioGeometry::SM64MarioGeometry()
{
    memset(position, 0, sizeof(position));
    memset(normal,   0, sizeof(normal));
    memset(color,    0, sizeof(color));
    memset(uv,       0, sizeof(uv));

    geometry.position = position;
    geometry.normal   = normal;
    geometry.color    = color;
    geometry.uv       = uv;

    geometry.numTrianglesUsed = 0;
}

SM64MarioGeometry::SM64MarioGeometry(const SM64MarioGeometry &other)
{
    memcpy(position, other.position, sizeof(position));
    memcpy(normal,   other.normal,   sizeof(normal));
    memcpy(color,    other.color,    sizeof(color));
    memcpy(uv,       other.uv,       sizeof(uv));

    geometry.position = position;
    geometry.normal   = normal;
    geometry.color    = color;
    geometry.uv       = uv;

    geometry.numTrianglesUsed = other.geometry.numTrianglesUsed;
}

struct SM64MarioGeometryBuffers &SM64MarioGeometry::c_handle()
{
    return geometry;
}