#include <libsm64_mario_geometry.hpp>

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
LibSM64MarioGeometry::LibSM64MarioGeometry() :
		geometry({ position.data(), normal.data(), color.data(), uv.data(), 0 }) {}
// NOLINTEND(cppcoreguidelines-pro-type-member-init)
