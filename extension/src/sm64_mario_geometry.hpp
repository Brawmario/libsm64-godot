#ifndef LIBSM64GD_SM64MARIOGEOMETRY_H
#define LIBSM64GD_SM64MARIOGEOMETRY_H

#include <array>

#include <godot_cpp/core/defs.hpp>

#include <libsm64.h>

class SM64MarioGeometry
{
public:
    SM64MarioGeometry();
    SM64MarioGeometry(const SM64MarioGeometry &other);

    SM64MarioGeometry &operator=(const SM64MarioGeometry &other);

    _FORCE_INLINE_ struct SM64MarioGeometryBuffers *c_handle() { return &geometry; }

    _FORCE_INLINE_ uint16_t triangles_used() const { return geometry.numTrianglesUsed; }

    void lerp(const SM64MarioGeometry &last, const SM64MarioGeometry &current, float amount);

    std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> position;
    std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> normal;
    std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> color;
    std::array<float, 6 * SM64_GEO_MAX_TRIANGLES> uv;

private:
    struct SM64MarioGeometryBuffers geometry;
};

#endif // LIBSM64GD_SM64MARIOGEOMETRY_H
