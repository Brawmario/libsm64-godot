#ifndef LIBSM64GD_LIBSM64MARIOINTERPOLATOR_H
#define LIBSM64GD_LIBSM64MARIOINTERPOLATOR_H

#include <godot_cpp/classes/ref.hpp>

#include <libsm64_mario_state.hpp>

#ifdef WIN32
#include <windows.h>
#endif

class LibSM64MarioInterpolator : public godot::RefCounted
{
    GDCLASS(LibSM64MarioInterpolator, godot::RefCounted);

public:
    LibSM64MarioInterpolator();

    void set_array_mesh_triangles_current(const godot::Array &p_value);
    godot::Array get_array_mesh_triangles_current() const;

    void set_array_mesh_triangles_previous(const godot::Array &p_value);
    godot::Array get_array_mesh_triangles_previous() const;

    void set_mario_state_current(const godot::Ref<LibSM64MarioState> &p_value);
    godot::Ref<LibSM64MarioState> get_mario_state_current() const;

    void set_mario_state_previous(const godot::Ref<LibSM64MarioState> &p_value);
    godot::Ref<LibSM64MarioState> get_mario_state_previous() const;

    godot::Array interolate_array_mesh_triangles(double p_t) const;
    godot::Ref<LibSM64MarioState> interpolate_mario_state(double p_t) const;

protected:
    static void _bind_methods();

private:
    godot::Array array_mesh_triangles_current;
    godot::Array array_mesh_triangles_previous;
    godot::Ref<LibSM64MarioState> mario_state_current;
    godot::Ref<LibSM64MarioState> mario_state_previous;
};

#endif // LIBSM64GD_LIBSM64MARIOINTERPOLATOR_H
