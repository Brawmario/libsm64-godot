#include <libsm64_mario_tick_output.hpp>

LibSM64MarioTickOutput::LibSM64MarioTickOutput(const godot::Ref<LibSM64MarioState> &p_mario_state, const godot::Array &p_array_mesh_triangles, const godot::Vector3 &p_array_mesh_offset, const godot::Vector3 &p_array_mesh_scale)
    : mario_state(p_mario_state),
      array_mesh_triangles(p_array_mesh_triangles),
      array_mesh_offset(p_array_mesh_offset),
      array_mesh_scale(p_array_mesh_scale)
{
}

void LibSM64MarioTickOutput::set_mario_state(const godot::Ref<LibSM64MarioState> &p_value)
{
    mario_state = p_value;
}

godot::Ref<LibSM64MarioState> LibSM64MarioTickOutput::get_mario_state() const
{
    return mario_state;
}

void LibSM64MarioTickOutput::set_array_mesh_triangles(const godot::Array &p_value)
{
    array_mesh_triangles = p_value;
}

godot::Array LibSM64MarioTickOutput::get_array_mesh_triangles() const
{
    return array_mesh_triangles;
}

void LibSM64MarioTickOutput::set_array_mesh_offset(const godot::Vector3 &p_value)
{
    array_mesh_offset = p_value;
}

godot::Vector3 LibSM64MarioTickOutput::get_array_mesh_offset() const
{
    return array_mesh_offset;
}

void LibSM64MarioTickOutput::set_array_mesh_scale(const godot::Vector3 &p_value)
{
    array_mesh_scale = p_value;
}

godot::Vector3 LibSM64MarioTickOutput::get_array_mesh_scale() const
{
    return array_mesh_scale;
}

void LibSM64MarioTickOutput::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_state", "value"), &LibSM64MarioTickOutput::set_mario_state);
    godot::ClassDB::bind_method(godot::D_METHOD("get_mario_state"), &LibSM64MarioTickOutput::get_mario_state);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "mario_state"), "set_mario_state", "get_mario_state");
    godot::ClassDB::bind_method(godot::D_METHOD("set_array_mesh_triangles", "value"), &LibSM64MarioTickOutput::set_array_mesh_triangles);
    godot::ClassDB::bind_method(godot::D_METHOD("get_array_mesh_triangles"), &LibSM64MarioTickOutput::get_array_mesh_triangles);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::ARRAY, "array_mesh_triangles"), "set_array_mesh_triangles", "get_array_mesh_triangles");
    godot::ClassDB::bind_method(godot::D_METHOD("set_array_mesh_offset", "value"), &LibSM64MarioTickOutput::set_array_mesh_offset);
    godot::ClassDB::bind_method(godot::D_METHOD("get_array_mesh_offset"), &LibSM64MarioTickOutput::get_array_mesh_offset);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::VECTOR3, "array_mesh_offset"), "set_array_mesh_offset", "get_array_mesh_offset");
    godot::ClassDB::bind_method(godot::D_METHOD("set_array_mesh_scale", "value"), &LibSM64MarioTickOutput::set_array_mesh_scale);
    godot::ClassDB::bind_method(godot::D_METHOD("get_array_mesh_scale"), &LibSM64MarioTickOutput::get_array_mesh_scale);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::VECTOR3, "array_mesh_scale"), "set_array_mesh_scale", "get_array_mesh_scale");
}
