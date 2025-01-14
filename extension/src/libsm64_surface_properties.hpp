#ifndef LIBSM64GD_LIBSM64SURFACEPROPERTIES_H
#define LIBSM64GD_LIBSM64SURFACEPROPERTIES_H

#include <godot_cpp/classes/resource.hpp>

#include <libsm64.hpp>

#ifdef WIN32
#include <windows.h>
#endif

class LibSM64SurfaceProperties : public godot::Resource
{
    GDCLASS(LibSM64SurfaceProperties, godot::Resource);

public:
    LibSM64SurfaceProperties() = default;

    void set_surface_type(SurfaceType value);
    SurfaceType get_surface_type() const;

    void set_terrain_type(TerrainType value);
    TerrainType get_terrain_type() const;

    void set_force(int value);
    int get_force() const;

protected:
    static void _bind_methods();

private:
    SurfaceType surface_type = SURFACE_DEFAULT;
    TerrainType terrain_type = TERRAIN_GRASS;
    int force = 0;
};

#endif // LIBSM64GD_LIBSM64SURFACEPROPERTIES_H
