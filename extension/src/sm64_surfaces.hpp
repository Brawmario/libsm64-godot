#ifndef LIBSM64GD_SM64SURFACES_H
#define LIBSM64GD_SM64SURFACES_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>

#include <sm64_surface_properties.hpp>

extern "C"
{
#include <libsm64.h>
}

#ifdef WIN32
#include <windows.h>
#endif

class SM64Surfaces : public godot::Object
{
    GDCLASS(SM64Surfaces, godot::Object);

    inline static SM64Surfaces *singleton = nullptr;

public:
    SM64Surfaces();
    ~SM64Surfaces();

    static SM64Surfaces *get_singleton();

    void static_surfaces_load(godot::PackedVector3Array p_vertexes, godot::TypedArray<SM64SurfaceProperties> p_surface_properties_array);

    int surface_object_create(godot::PackedVector3Array p_vertexes, godot::Vector3 p_position, godot::Vector3 p_rotation, godot::TypedArray<SM64SurfaceProperties> p_suface_properties_array);
    void surface_object_move(int p_object_id, godot::Vector3 p_position, godot::Vector3 p_rotation);
    void surface_object_delete(int p_object_id);

protected:
    static void _bind_methods();
};

#endif // LIBSM64GD_SM64SURFACES_H