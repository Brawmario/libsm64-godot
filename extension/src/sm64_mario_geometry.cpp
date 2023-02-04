#include <sm64_mario_geometry.hpp>

#include <algorithm>

#define LERP(last, current, amount) (last + (current - last) * amount)

SM64MarioGeometry::SM64MarioGeometry()
{
    std::fill(position, position + (9 * SM64_GEO_MAX_TRIANGLES), 0.0f);
    std::fill(normal,   normal   + (9 * SM64_GEO_MAX_TRIANGLES), 0.0f);
    std::fill(color,    color    + (9 * SM64_GEO_MAX_TRIANGLES), 0.0f);
    std::fill(uv,       uv       + (6 * SM64_GEO_MAX_TRIANGLES), 0.0f);

    geometry.position = position;
    geometry.normal   = normal;
    geometry.color    = color;
    geometry.uv       = uv;

    geometry.numTrianglesUsed = 0;
}

SM64MarioGeometry::SM64MarioGeometry(const SM64MarioGeometry &other)
{
    std::copy(other.position, other.position + (9 * SM64_GEO_MAX_TRIANGLES), position);
    std::copy(other.normal,   other.normal   + (9 * SM64_GEO_MAX_TRIANGLES), normal);
    std::copy(other.color,    other.color    + (9 * SM64_GEO_MAX_TRIANGLES), color);
    std::copy(other.uv,       other.uv       + (6 * SM64_GEO_MAX_TRIANGLES), uv);

    geometry.position = position;
    geometry.normal   = normal;
    geometry.color    = color;
    geometry.uv       = uv;

    geometry.numTrianglesUsed = other.geometry.numTrianglesUsed;
}

SM64MarioGeometry &SM64MarioGeometry::operator=(const SM64MarioGeometry &other)
{
    std::copy(other.position, other.position + (9 * SM64_GEO_MAX_TRIANGLES), position);
    std::copy(other.normal,   other.normal   + (9 * SM64_GEO_MAX_TRIANGLES), normal);
    std::copy(other.color,    other.color    + (9 * SM64_GEO_MAX_TRIANGLES), color);
    std::copy(other.uv,       other.uv       + (6 * SM64_GEO_MAX_TRIANGLES), uv);

    geometry.numTrianglesUsed = other.geometry.numTrianglesUsed;

    return *this;
}

void SM64MarioGeometry::lerp(const SM64MarioGeometry &last, const SM64MarioGeometry &current, float amount)
{
    geometry.numTrianglesUsed = current.geometry.numTrianglesUsed;

    for (int i = 0; i < 9 * geometry.numTrianglesUsed; i++)
        position[i] = LERP(last.position[i], current.position[i], amount);

    for (int i = 0; i < 9 * geometry.numTrianglesUsed; i++)
        normal[i] = LERP(last.normal[i], current.normal[i], amount);

    // Mario's colors seems to be always constant, no need to interpolate.
    // for (int i = 0; i < 9 * geometry.numTrianglesUsed; i++)
    //     color[i] = LERP(last.color[i], current.color[i], amount);
    std::copy(current.color, current.color + (9 * SM64_GEO_MAX_TRIANGLES), color);

    for (int i = 0; i < 6 * geometry.numTrianglesUsed; i++)
        uv[i] = LERP(last.uv[i], current.uv[i], amount);
}