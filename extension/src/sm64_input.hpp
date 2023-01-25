#ifndef LIBSM64GD_SM64INPUT_H
#define LIBSM64GD_SM64INPUT_H

#include <godot_cpp/classes/ref.hpp>

#ifdef WIN32
#include <windows.h>
#endif

class SM64Input : public godot::RefCounted
{
    GDCLASS(SM64Input, godot::RefCounted);

public:
    SM64Input() = default;

    void set_cam_look(godot::Vector2 value);
    godot::Vector2 get_cam_look() const;

    void set_stick(godot::Vector2 value);
    godot::Vector2 get_stick() const;

    void set_a(bool value);
    bool get_a() const;

    void set_b(bool value);
    bool get_b() const;

    void set_z(bool value);
    bool get_z() const;

protected:
    static void _bind_methods();

private:
    godot::Vector2 cam_look;
    godot::Vector2 stick;
    bool a = false;
    bool b = false;
    bool z = false;
};

#endif // LIBSM64GD_SM64INPUT_H