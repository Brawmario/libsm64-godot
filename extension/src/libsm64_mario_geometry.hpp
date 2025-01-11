#ifndef LIBSM64GD_LIBSM64MARIOGEOMETRY_H
#define LIBSM64GD_LIBSM64MARIOGEOMETRY_H

#include <array>

#include <godot_cpp/core/defs.hpp>

#include <libsm64.h>

class LibSM64MarioGeometry
{
public:
    LibSM64MarioGeometry();

    _FORCE_INLINE_ struct SM64MarioGeometryBuffers *data() { return &geometry; }
    _FORCE_INLINE_ int triangles() const { return geometry.numTrianglesUsed; }

    std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> position;
    std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> normal;
    std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> color;
    std::array<float, 6 * SM64_GEO_MAX_TRIANGLES> uv;

private:
    struct SM64MarioGeometryBuffers geometry;
};

#endif // LIBSM64GD_LIBSM64MARIOGEOMETRY_H
