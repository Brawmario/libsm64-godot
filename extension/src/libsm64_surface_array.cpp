#include <libsm64_surface_array.hpp>

#include <libsm64.hpp>

void LibSM64SurfaceArray::append_surfaces(const godot::Ref<LibSM64SurfaceArray> &p_surfaces)
{
    ERR_FAIL_NULL(p_surfaces);

    const auto &sm64_surfaces_other = p_surfaces->sm64_surfaces;
    sm64_surfaces.reserve(sm64_surfaces.size() + sm64_surfaces_other.size());
    sm64_surfaces.insert(sm64_surfaces.end(), sm64_surfaces_other.begin(), sm64_surfaces_other.end());
}

void LibSM64SurfaceArray::add_triangle(const godot::Vector3 &p_vertex_1, const godot::Vector3 &p_vertex_2, const godot::Vector3 &p_vertex_3, int p_surface_type, int p_terrain_type, int p_force)
{
    LibSM64 *libsm64 = LibSM64::get_singleton();
    ERR_FAIL_NULL(libsm64);
    const auto scale_factor = libsm64->get_scale_factor();

    sm64_surfaces.push_back({
        p_surface_type,
        p_force,
        p_terrain_type,
        {
            {p_vertex_2.z * scale_factor, p_vertex_2.y * scale_factor, -p_vertex_2.x * scale_factor},
            {p_vertex_1.z * scale_factor, p_vertex_1.y * scale_factor, -p_vertex_1.x * scale_factor},
            {p_vertex_3.z * scale_factor, p_vertex_3.y * scale_factor, -p_vertex_3.x * scale_factor},
        },
    });

}

void LibSM64SurfaceArray::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("append_surfaces", "surfaces"), &LibSM64SurfaceArray::append_surfaces);
    godot::ClassDB::bind_method(godot::D_METHOD("add_triangle", "vertex_1", "vertex_2", "vertex_3", "surface_type", "terrain_type", "force"), &LibSM64SurfaceArray::add_triangle, DEFVAL(0), DEFVAL(0), DEFVAL(0));
}
