#ifndef LIBSM64GD_LIBSM64SURFACEARRAY_H
#define LIBSM64GD_LIBSM64SURFACEARRAY_H

#include <vector>

#include <godot_cpp/classes/ref.hpp>

#include <libsm64.h>

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

    void add_triangle(const godot::Vector3 &p_vertex_1, const godot::Vector3 &p_vertex_2, const godot::Vector3 &p_vertex_3, int16_t p_surface_type = 0, uint16_t p_terrain_type = 0, int16_t p_force = 0);

protected:
    static void _bind_methods();

private:
    std::vector<struct SM64Surface> sm64_surfaces;
};

#endif // LIBSM64GD_LIBSM64SURFACEARRAY_H
