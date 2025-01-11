#ifndef LIBSM64GD_LIBSM64MARIOINPUTS_H
#define LIBSM64GD_LIBSM64MARIOINPUTS_H

#include <godot_cpp/classes/ref.hpp>

#include <libsm64.h>

#ifdef WIN32
#include <windows.h>
#endif

class LibSM64MarioInputs : public godot::RefCounted
{
    GDCLASS(LibSM64MarioInputs, godot::RefCounted);

public:
    LibSM64MarioInputs() = default;

    void set_cam_look(godot::Vector2 value);
    godot::Vector2 get_cam_look() const;

    void set_stick(godot::Vector2 value);
    godot::Vector2 get_stick() const;

    void set_button_a(bool value);
    bool get_button_a() const;

    void set_button_b(bool value);
    bool get_button_b() const;

    void set_button_z(bool value);
    bool get_button_z() const;

    _FORCE_INLINE_ struct SM64MarioInputs to_sm64() const
    {
        return {
            -cam_look.y,
            cam_look.x,
            stick.x,
            stick.y,
            button_a,
            button_b,
            button_z
        };
    }

protected:
    static void _bind_methods();

private:
    godot::Vector2 cam_look;
    godot::Vector2 stick;
    bool button_a = false;
    bool button_b = false;
    bool button_z = false;
};

#endif // LIBSM64GD_LIBSM64MARIOINPUTS_H
