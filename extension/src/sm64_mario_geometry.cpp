#include <sm64_mario_geometry.hpp>

#include <string.h>

static float lerp(float a, float b, float amount)
{
    return a + (b - a) * amount;
}

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

SM64MarioGeometry &SM64MarioGeometry::operator=(const SM64MarioGeometry &other)
{
    memcpy(position, other.position, sizeof(position));
    memcpy(normal,   other.normal,   sizeof(normal));
    memcpy(color,    other.color,    sizeof(color));
    memcpy(uv,       other.uv,       sizeof(uv));

    geometry.numTrianglesUsed = other.geometry.numTrianglesUsed;

    return *this;
}

void SM64MarioGeometry::lerp(const SM64MarioGeometry &last, const SM64MarioGeometry &current, float amount)
{
    geometry.numTrianglesUsed = current.geometry.numTrianglesUsed;

    for (int i = 0; i < 9 * geometry.numTrianglesUsed; i++)
        position[i] = ::lerp(last.position[i], current.position[i], amount);

    for (int i = 0; i < 9 * geometry.numTrianglesUsed; i++)
        normal[i] = ::lerp(last.normal[i], current.normal[i], amount);

    // Mario's colors seems to be always constant, no need to interpolate.
    // for (int i = 0; i < 9 * geometry.numTrianglesUsed; i++)
    //     color[i] = ::lerp(last.color[i], current.color[i], amount);
    memcpy(color, last.color, sizeof(color));

    for (int i = 0; i < 6 * geometry.numTrianglesUsed; i++)
        uv[i] = ::lerp(last.uv[i], current.uv[i], amount);
}