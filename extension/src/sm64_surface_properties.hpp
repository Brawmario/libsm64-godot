#ifndef LIBSM64GD_SM64SURFACEPROPERTIES_H
#define LIBSM64GD_SM64SURFACEPROPERTIES_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/binder_common.hpp>

#ifdef WIN32
#include <windows.h>
#endif

class SM64SurfaceProperties : public godot::Resource
{
    GDCLASS(SM64SurfaceProperties, godot::Resource);

public:
    SM64SurfaceProperties() = default;

    void set_surface_type(int value);
    int get_surface_type() const;

    void set_terrain_type(int value);
    int get_terrain_type() const;

    void set_force(int value);
    int get_force() const;

protected:
    static void _bind_methods();

private:
    int surface_type = 0;
    int terrain_type = 0;
    int force = 0;
};

#endif // LIBSM64GD_SM64SURFACEPROPERTIES_H
