#ifndef LIBSM64GD_LIBSM64MARIOGEOMETRY_H
#define LIBSM64GD_LIBSM64MARIOGEOMETRY_H

#include <array>

#include <godot_cpp/core/defs.hpp>

#include <libsm64.h>

class LibSM64MarioGeometry {
public:
	// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
	_FORCE_INLINE_ LibSM64MarioGeometry() :
			geometry({ position.data(), normal.data(), color.data(), uv.data(), 0 }) {}
	// NOLINTEND(cppcoreguidelines-pro-type-member-init)

	_FORCE_INLINE_ constexpr struct SM64MarioGeometryBuffers *data() { return &geometry; }
	_FORCE_INLINE_ constexpr int triangles() const { return geometry.numTrianglesUsed; }

	std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> position;
	std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> normal;
	std::array<float, 9 * SM64_GEO_MAX_TRIANGLES> color;
	std::array<float, 6 * SM64_GEO_MAX_TRIANGLES> uv;

private:
	struct SM64MarioGeometryBuffers geometry;
};

#endif // LIBSM64GD_LIBSM64MARIOGEOMETRY_H
