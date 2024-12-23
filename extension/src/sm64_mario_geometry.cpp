#include <sm64_mario_geometry.hpp>

#include <algorithm>

SM64MarioGeometry::SM64MarioGeometry()
{
    position.fill(0.0f);
    normal.fill(0.0f);
    color.fill(0.0f);
    uv.fill(0.0f);

    geometry.position = position.data();
    geometry.normal = normal.data();
    geometry.color = color.data();
    geometry.uv = uv.data();

    geometry.numTrianglesUsed = 0;
}

SM64MarioGeometry::SM64MarioGeometry(const SM64MarioGeometry &other)
{
    position = other.position;
    normal = other.normal;
    color = other.color;
    uv = other.uv;

    geometry.position = position.data();
    geometry.normal = normal.data();
    geometry.color = color.data();
    geometry.uv = uv.data();

    geometry.numTrianglesUsed = other.geometry.numTrianglesUsed;
}

SM64MarioGeometry &SM64MarioGeometry::operator=(const SM64MarioGeometry &other)
{
    position = other.position;
    normal = other.normal;
    color = other.color;
    uv = other.uv;

    geometry.numTrianglesUsed = other.geometry.numTrianglesUsed;

    return *this;
}

#define LERP(last, current, amount) (last + (current - last) * amount)

template<class ConstIt, class It>
_FORCE_INLINE_ static void lerp_range(ConstIt p_it_last, ConstIt p_it_current, float p_amount, It p_it, It p_end)
{
    for (; p_it != p_end; ++p_it_last, ++p_it_current, ++p_it)
        *p_it = LERP(*p_it_last, *p_it_current, p_amount);
}

void SM64MarioGeometry::lerp(const SM64MarioGeometry &last, const SM64MarioGeometry &current, float amount)
{
    geometry.numTrianglesUsed = current.geometry.numTrianglesUsed;

    lerp_range(last.position.begin(), current.position.begin(), amount, position.begin(), position.begin() + geometry.numTrianglesUsed * 9);
    lerp_range(last.normal.begin(), current.normal.begin(), amount, normal.begin(), normal.begin() + geometry.numTrianglesUsed * 9);
    color = current.color;
    uv = current.uv;
}
