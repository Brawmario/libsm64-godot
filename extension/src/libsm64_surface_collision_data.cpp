#include <libsm64_surface_collision_data.hpp>

#include <godot_cpp/core/class_db.hpp>

// SM64 vertex (x, y, z) -> Godot vector (-z, y, x), converted to Godot's units.
static _FORCE_INLINE_ godot::Vector3 sm64_vertex_to_godot(const int32_t p_vertex[3], godot::real_t p_scale_factor) {
	return godot::Vector3(
			-static_cast<godot::real_t>(p_vertex[2]) / p_scale_factor,
			static_cast<godot::real_t>(p_vertex[1]) / p_scale_factor,
			static_cast<godot::real_t>(p_vertex[0]) / p_scale_factor);
}

LibSM64SurfaceCollisionData::LibSM64SurfaceCollisionData(const struct SM64SurfaceCollisionData &p_data, godot::real_t p_scale_factor) :
		surface_type(static_cast<LibSM64::SurfaceType>(p_data.type)),
		terrain_type(static_cast<LibSM64::TerrainType>(p_data.terrain)),
		force(p_data.force),
		flags(p_data.flags),
		room(p_data.room),
		valid(p_data.isValid != 0),
		lower_y(static_cast<godot::real_t>(p_data.lowerY) / p_scale_factor),
		upper_y(static_cast<godot::real_t>(p_data.upperY) / p_scale_factor),
		normal(-p_data.normal.z, p_data.normal.y, p_data.normal.x),
		origin_offset(static_cast<godot::real_t>(p_data.originOffset) / p_scale_factor) {
	vertices.resize(3);
	vertices.set(0, sm64_vertex_to_godot(p_data.vertex1, p_scale_factor));
	vertices.set(1, sm64_vertex_to_godot(p_data.vertex2, p_scale_factor));
	vertices.set(2, sm64_vertex_to_godot(p_data.vertex3, p_scale_factor));
}

LibSM64::SurfaceType LibSM64SurfaceCollisionData::get_surface_type() const {
	return surface_type;
}

LibSM64::TerrainType LibSM64SurfaceCollisionData::get_terrain_type() const {
	return terrain_type;
}

int LibSM64SurfaceCollisionData::get_force() const {
	return force;
}

int LibSM64SurfaceCollisionData::get_flags() const {
	return flags;
}

int LibSM64SurfaceCollisionData::get_room() const {
	return room;
}

bool LibSM64SurfaceCollisionData::get_valid() const {
	return valid;
}

godot::real_t LibSM64SurfaceCollisionData::get_lower_y() const {
	return lower_y;
}

godot::real_t LibSM64SurfaceCollisionData::get_upper_y() const {
	return upper_y;
}

godot::PackedVector3Array LibSM64SurfaceCollisionData::get_vertices() const {
	return vertices;
}

godot::Vector3 LibSM64SurfaceCollisionData::get_normal() const {
	return normal;
}

godot::real_t LibSM64SurfaceCollisionData::get_origin_offset() const {
	return origin_offset;
}

void LibSM64SurfaceCollisionData::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_surface_type"), &LibSM64SurfaceCollisionData::get_surface_type);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "surface_type"), "", "get_surface_type");
	godot::ClassDB::bind_method(godot::D_METHOD("get_terrain_type"), &LibSM64SurfaceCollisionData::get_terrain_type);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "terrain_type"), "", "get_terrain_type");
	godot::ClassDB::bind_method(godot::D_METHOD("get_force"), &LibSM64SurfaceCollisionData::get_force);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "force"), "", "get_force");
	godot::ClassDB::bind_method(godot::D_METHOD("get_flags"), &LibSM64SurfaceCollisionData::get_flags);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "flags"), "", "get_flags");
	godot::ClassDB::bind_method(godot::D_METHOD("get_room"), &LibSM64SurfaceCollisionData::get_room);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "room"), "", "get_room");
	godot::ClassDB::bind_method(godot::D_METHOD("get_valid"), &LibSM64SurfaceCollisionData::get_valid);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "valid"), "", "get_valid");
	godot::ClassDB::bind_method(godot::D_METHOD("get_lower_y"), &LibSM64SurfaceCollisionData::get_lower_y);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "lower_y"), "", "get_lower_y");
	godot::ClassDB::bind_method(godot::D_METHOD("get_upper_y"), &LibSM64SurfaceCollisionData::get_upper_y);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "upper_y"), "", "get_upper_y");
	godot::ClassDB::bind_method(godot::D_METHOD("get_vertices"), &LibSM64SurfaceCollisionData::get_vertices);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::PACKED_VECTOR3_ARRAY, "vertices"), "", "get_vertices");
	godot::ClassDB::bind_method(godot::D_METHOD("get_normal"), &LibSM64SurfaceCollisionData::get_normal);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::VECTOR3, "normal"), "", "get_normal");
	godot::ClassDB::bind_method(godot::D_METHOD("get_origin_offset"), &LibSM64SurfaceCollisionData::get_origin_offset);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "origin_offset"), "", "get_origin_offset");
}
