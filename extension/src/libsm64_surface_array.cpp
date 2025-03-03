#include <libsm64_surface_array.hpp>

void LibSM64SurfaceArray::append_surfaces(const godot::Ref<LibSM64SurfaceArray> &p_surfaces) {
	ERR_FAIL_NULL(p_surfaces);

	const auto &sm64_surfaces_other = p_surfaces->sm64_surfaces;
	sm64_surfaces.reserve(sm64_surfaces.size() + sm64_surfaces_other.size());
	sm64_surfaces.insert(sm64_surfaces.end(), sm64_surfaces_other.begin(), sm64_surfaces_other.end());
}

void LibSM64SurfaceArray::add_triangle(const godot::Vector3 &p_vertex_1, const godot::Vector3 &p_vertex_2, const godot::Vector3 &p_vertex_3, LibSM64::SurfaceType p_surface_type, LibSM64::TerrainType p_terrain_type, int p_force) {
	LibSM64 *libsm64 = LibSM64::get_singleton();
	ERR_FAIL_NULL(libsm64);
	const auto scale_factor = libsm64->get_scale_factor();

	sm64_surfaces.push_back({
			static_cast<int16_t>(p_surface_type),
			static_cast<int16_t>(p_force),
			static_cast<uint16_t>(p_terrain_type),
			{
					{ static_cast<int32_t>(p_vertex_2.z * scale_factor),
							static_cast<int32_t>(p_vertex_2.y * scale_factor),
							static_cast<int32_t>(-p_vertex_2.x * scale_factor) },
					{ static_cast<int32_t>(p_vertex_1.z * scale_factor),
							static_cast<int32_t>(p_vertex_1.y * scale_factor),
							static_cast<int32_t>(-p_vertex_1.x * scale_factor) },
					{ static_cast<int32_t>(p_vertex_3.z * scale_factor),
							static_cast<int32_t>(p_vertex_3.y * scale_factor),
							static_cast<int32_t>(-p_vertex_3.x * scale_factor) },
			},
	});
}

void LibSM64SurfaceArray::add_triangles(const godot::PackedVector3Array &p_vertices, LibSM64::SurfaceType p_surface_type, LibSM64::TerrainType p_terrain_type, int p_force) {
	const auto *vertices_ptr = p_vertices.ptr();
	const auto vertices_size = p_vertices.size();
	ERR_FAIL_COND(vertices_size % 3 != 0);

	LibSM64 *libsm64 = LibSM64::get_singleton();
	ERR_FAIL_NULL(libsm64);
	const auto scale_factor = libsm64->get_scale_factor();

	for (int i = 0; i < vertices_size; i += 3) {
		sm64_surfaces.push_back({
				static_cast<int16_t>(p_surface_type),
				static_cast<int16_t>(p_force),
				static_cast<uint16_t>(p_terrain_type),
				{
						{ static_cast<int32_t>(vertices_ptr[i + 1].z * scale_factor),
								static_cast<int32_t>(vertices_ptr[i + 1].y * scale_factor),
								static_cast<int32_t>(-vertices_ptr[i + 1].x * scale_factor) },
						{ static_cast<int32_t>(vertices_ptr[i + 0].z * scale_factor),
								static_cast<int32_t>(vertices_ptr[i + 0].y * scale_factor),
								static_cast<int32_t>(-vertices_ptr[i + 0].x * scale_factor) },
						{ static_cast<int32_t>(vertices_ptr[i + 2].z * scale_factor),
								static_cast<int32_t>(vertices_ptr[i + 2].y * scale_factor),
								static_cast<int32_t>(-vertices_ptr[i + 2].x * scale_factor) },
				},
		});
	}
}

void LibSM64SurfaceArray::add_triangle_with_properties(const godot::Vector3 &p_vertex_1, const godot::Vector3 &p_vertex_2, const godot::Vector3 &p_vertex_3, const godot::Ref<LibSM64SurfaceProperties> &p_properties) {
	ERR_FAIL_NULL(p_properties);
	add_triangle(p_vertex_1, p_vertex_2, p_vertex_3, p_properties->get_surface_type(), p_properties->get_terrain_type(), p_properties->get_force());
}

void LibSM64SurfaceArray::add_triangles_with_properties(const godot::PackedVector3Array &p_vertices, const godot::Ref<LibSM64SurfaceProperties> &p_properties) {
	ERR_FAIL_NULL(p_properties);
	add_triangles(p_vertices, p_properties->get_surface_type(), p_properties->get_terrain_type(), p_properties->get_force());
}

void LibSM64SurfaceArray::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("append_surfaces", "surfaces"), &LibSM64SurfaceArray::append_surfaces);
	godot::ClassDB::bind_method(godot::D_METHOD("add_triangle", "vertex_1", "vertex_2", "vertex_3", "surface_type", "terrain_type", "force"), &LibSM64SurfaceArray::add_triangle, DEFVAL(LibSM64::SurfaceType::SURFACE_DEFAULT), DEFVAL(LibSM64::TerrainType::TERRAIN_GRASS), DEFVAL(0));
	godot::ClassDB::bind_method(godot::D_METHOD("add_triangles", "vertices", "surface_type", "terrain_type", "force"), &LibSM64SurfaceArray::add_triangles, DEFVAL(LibSM64::SurfaceType::SURFACE_DEFAULT), DEFVAL(LibSM64::TerrainType::TERRAIN_GRASS), DEFVAL(0));
	godot::ClassDB::bind_method(godot::D_METHOD("add_triangle_with_properties", "vertex_1", "vertex_2", "vertex_3", "properties"), &LibSM64SurfaceArray::add_triangle_with_properties);
	godot::ClassDB::bind_method(godot::D_METHOD("add_triangles_with_properties", "vertices", "properties"), &LibSM64SurfaceArray::add_triangles_with_properties);
}
