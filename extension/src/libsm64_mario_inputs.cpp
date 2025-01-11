#include <libsm64_mario_inputs.hpp>

#include <godot_cpp/core/class_db.hpp>


void LibSM64MarioInputs::set_cam_look(godot::Vector2 value)
{
    cam_look = value;
}

godot::Vector2 LibSM64MarioInputs::get_cam_look() const
{
    return cam_look;
}

void LibSM64MarioInputs::set_stick(godot::Vector2 value)
{
    stick = value;
}

godot::Vector2 LibSM64MarioInputs::get_stick() const
{
    return stick;
}

void LibSM64MarioInputs::set_button_a(bool value)
{
    button_a = value;
}

bool LibSM64MarioInputs::get_button_a() const
{
    return button_a;
}

void LibSM64MarioInputs::set_button_b(bool value)
{
    button_b = value;
}

bool LibSM64MarioInputs::get_button_b() const
{
    return button_b;
}

void LibSM64MarioInputs::set_button_z(bool value)
{
    button_z = value;
}

bool LibSM64MarioInputs::get_button_z() const
{
    return button_z;
}

void LibSM64MarioInputs::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_cam_look", "value"), &LibSM64MarioInputs::set_cam_look);
    godot::ClassDB::bind_method(godot::D_METHOD("get_cam_look"), &LibSM64MarioInputs::get_cam_look);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::VECTOR2, "cam_look"), "set_cam_look", "get_cam_look");
    godot::ClassDB::bind_method(godot::D_METHOD("set_stick", "value"), &LibSM64MarioInputs::set_stick);
    godot::ClassDB::bind_method(godot::D_METHOD("get_stick"), &LibSM64MarioInputs::get_stick);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::VECTOR2, "stick"), "set_stick", "get_stick");
    godot::ClassDB::bind_method(godot::D_METHOD("set_button_a", "value"), &LibSM64MarioInputs::set_button_a);
    godot::ClassDB::bind_method(godot::D_METHOD("get_button_a"), &LibSM64MarioInputs::get_button_a);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "button_a"), "set_button_a", "get_button_a");
    godot::ClassDB::bind_method(godot::D_METHOD("set_button_b", "value"), &LibSM64MarioInputs::set_button_b);
    godot::ClassDB::bind_method(godot::D_METHOD("get_button_b"), &LibSM64MarioInputs::get_button_b);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "button_b"), "set_button_b", "get_button_b");
    godot::ClassDB::bind_method(godot::D_METHOD("set_button_z", "value"), &LibSM64MarioInputs::set_button_z);
    godot::ClassDB::bind_method(godot::D_METHOD("get_button_z"), &LibSM64MarioInputs::get_button_z);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "button_z"), "set_button_z", "get_button_z");
}
