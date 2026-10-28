#ifndef LIBSM64GD_LIBSM64SURFACECOLLISIONDATA_H
#define LIBSM64GD_LIBSM64SURFACECOLLISIONDATA_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <libsm64.hpp>

// Read-only snapshot of a surface returned by the collision queries of LibSM64 (see
// LibSM64::surface_find_floor, LibSM64::surface_find_ceil and LibSM64::surface_find_wall_collisions).
// The engine's SM64SurfaceCollisionData cannot be exposed directly because it holds a pointer into
// libsm64's internal heap, so only its value fields are copied here, converted to Godot's units.
class LibSM64SurfaceCollisionData : public godot::RefCounted {
	GDCLASS(LibSM64SurfaceCollisionData, godot::RefCounted);

public:
	LibSM64SurfaceCollisionData() = default;
	LibSM64SurfaceCollisionData(const struct SM64SurfaceCollisionData &p_data, godot::real_t p_scale_factor);

	LibSM64::SurfaceType get_surface_type() const;
	LibSM64::TerrainType get_terrain_type() const;
	int get_force() const;
	int get_flags() const;
	int get_room() const;
	bool get_valid() const;
	godot::real_t get_lower_y() const;
	godot::real_t get_upper_y() const;
	godot::PackedVector3Array get_vertices() const;
	godot::Vector3 get_normal() const;
	godot::real_t get_origin_offset() const;

protected:
	static void _bind_methods();

private:
	LibSM64::SurfaceType surface_type = LibSM64::SurfaceType::SURFACE_DEFAULT;
	LibSM64::TerrainType terrain_type = LibSM64::TerrainType::TERRAIN_GRASS;
	int force = 0;
	int flags = 0;
	int room = 0;
	bool valid = false;
	godot::real_t lower_y = 0.0;
	godot::real_t upper_y = 0.0;
	godot::PackedVector3Array vertices;
	godot::Vector3 normal;
	godot::real_t origin_offset = 0.0;
};

#endif // LIBSM64GD_LIBSM64SURFACECOLLISIONDATA_H
