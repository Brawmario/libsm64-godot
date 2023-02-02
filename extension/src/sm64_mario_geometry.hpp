#ifndef LIBSM64GD_SM64MARIOGEOMETRY_H
#define LIBSM64GD_SM64MARIOGEOMETRY_H

extern "C"
{
#include <libsm64.h>
}

class SM64MarioGeometry
{
public:
    SM64MarioGeometry();
    SM64MarioGeometry(const SM64MarioGeometry &other);

    struct SM64MarioGeometryBuffers &c_handle();

    void lerp(const SM64MarioGeometry &previous, float amount);

private:
    struct SM64MarioGeometryBuffers geometry;

    float position[9 * SM64_GEO_MAX_TRIANGLES];
    float normal[9 * SM64_GEO_MAX_TRIANGLES];
    float color[9 * SM64_GEO_MAX_TRIANGLES];
    float uv[6 * SM64_GEO_MAX_TRIANGLES];
};

#endif // LIBSM64GD_SM64MARIOGEOMETRY_H