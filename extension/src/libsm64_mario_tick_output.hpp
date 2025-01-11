#ifndef LIBSM64GD_LIBSM64MARIOTICKOUTPUT_H
#define LIBSM64GD_LIBSM64MARIOTICKOUTPUT_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>

#include <libsm64_mario_state.hpp>

class LibSM64MarioTickOutput : public godot::RefCounted
{
    GDCLASS(LibSM64MarioTickOutput, godot::RefCounted);

public:
    LibSM64MarioTickOutput() = default;
    LibSM64MarioTickOutput(const godot::Ref<LibSM64MarioState> &p_mario_state, const godot::Array &p_array_mesh_triangles, const godot::Vector3 &p_array_mesh_offset, const godot::Vector3 &p_array_mesh_scale);

    void set_mario_state(const godot::Ref<LibSM64MarioState> &p_value);
    godot::Ref<LibSM64MarioState> get_mario_state() const;

    void set_array_mesh_triangles(const godot::Array &p_value);
    godot::Array get_array_mesh_triangles() const;

    void set_array_mesh_offset(const godot::Vector3 &p_value);
    godot::Vector3 get_array_mesh_offset() const;

    void set_array_mesh_scale(const godot::Vector3 &p_value);
    godot::Vector3 get_array_mesh_scale() const;

protected:
    static void _bind_methods();

private:
    godot::Ref<LibSM64MarioState> mario_state;
    godot::Array array_mesh_triangles;
    godot::Vector3 array_mesh_offset;
    godot::Vector3 array_mesh_scale;
};

#endif // LIBSM64GD_LIBSM64MARIOTICKOUTPUT_H
