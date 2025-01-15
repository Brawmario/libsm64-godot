#ifndef LIBSM64GD_LIBSM64SURFACEARRAY_H
#define LIBSM64GD_LIBSM64SURFACEARRAY_H

#include <vector>

#include <godot_cpp/classes/ref.hpp>

#include <libsm64_surface_properties.hpp>

#ifdef WIN32
#include <windows.h>
#endif

class LibSM64SurfaceArray : public godot::RefCounted
{
    GDCLASS(LibSM64SurfaceArray, godot::RefCounted);

    friend class LibSM64;

public:
    LibSM64SurfaceArray() = default;

    void append_surfaces(const godot::Ref<LibSM64SurfaceArray> &p_surfaces);

    void add_triangle(const godot::Vector3 &p_vertex_1, const godot::Vector3 &p_vertex_2, const godot::Vector3 &p_vertex_3, LibSM64::SurfaceType p_surface_type = LibSM64::SurfaceType::SURFACE_DEFAULT, LibSM64::TerrainType p_terrain_type = LibSM64::TerrainType::TERRAIN_GRASS, int p_force = 0);
    void add_triangle_with_properties(const godot::Vector3 &p_vertex_1, const godot::Vector3 &p_vertex_2, const godot::Vector3 &p_vertex_3, const godot::Ref<LibSM64SurfaceProperties> &p_properties);

protected:
    static void _bind_methods();

private:
    std::vector<struct SM64Surface> sm64_surfaces;
};

#endif // LIBSM64GD_LIBSM64SURFACEARRAY_H
