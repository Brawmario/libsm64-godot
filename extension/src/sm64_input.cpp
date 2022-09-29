#include <sm64_input.hpp>

#include <godot_cpp/core/class_db.hpp>


void SM64Input::set_cam_look(godot::Vector2 value)
{
    cam_look = value;
}

godot::Vector2 SM64Input::get_cam_look() const
{
    return cam_look;
}

void SM64Input::set_stick(godot::Vector2 value)
{
    stick = value;
}

godot::Vector2 SM64Input::get_stick() const
{
    return stick;
}

void SM64Input::set_a(bool value)
{
    a = value;
}

bool SM64Input::get_a() const
{
    return a;
}

void SM64Input::set_b(bool value)
{
    b = value;
}

bool SM64Input::get_b() const
{
    return b;
}

void SM64Input::set_z(bool value)
{
    z = value;
}

bool SM64Input::get_z() const
{
    return z;
}

void SM64Input::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_cam_look", "value"), &SM64Input::set_cam_look);
    godot::ClassDB::bind_method(godot::D_METHOD("get_cam_look"), &SM64Input::get_cam_look);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::VECTOR2, "cam_look"), "set_cam_look", "get_cam_look");
    godot::ClassDB::bind_method(godot::D_METHOD("set_stick", "value"), &SM64Input::set_stick);
    godot::ClassDB::bind_method(godot::D_METHOD("get_stick"), &SM64Input::get_stick);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::VECTOR2, "stick"), "set_stick", "get_stick");
    godot::ClassDB::bind_method(godot::D_METHOD("set_a", "value"), &SM64Input::set_a);
    godot::ClassDB::bind_method(godot::D_METHOD("get_a"), &SM64Input::get_a);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "a"), "set_a", "get_a");
    godot::ClassDB::bind_method(godot::D_METHOD("set_b", "value"), &SM64Input::set_b);
    godot::ClassDB::bind_method(godot::D_METHOD("get_b"), &SM64Input::get_b);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "b"), "set_b", "get_b");
    godot::ClassDB::bind_method(godot::D_METHOD("set_z", "value"), &SM64Input::set_z);
    godot::ClassDB::bind_method(godot::D_METHOD("get_z"), &SM64Input::get_z);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "z"), "set_z", "get_z");
}
