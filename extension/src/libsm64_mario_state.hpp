#ifndef LIBSM64GD_LIBSM64MARIOSTATE_H
#define LIBSM64GD_LIBSM64MARIOSTATE_H

#include <godot_cpp/classes/ref.hpp>

#include <libsm64.h>

#ifdef WIN32
#include <windows.h>
#endif

class LibSM64MarioState : public godot::RefCounted
{
    GDCLASS(LibSM64MarioState, godot::RefCounted);

public:
    LibSM64MarioState();
    LibSM64MarioState(const struct SM64MarioState &state, real_t scale_factor);

    void set_position(godot::Vector3 p_value);
    godot::Vector3 get_position() const;

    void set_velocity(godot::Vector3 p_value);
    godot::Vector3 get_velocity() const;

    void set_face_angle(float p_value);
    float get_face_angle() const;

    void set_health(int p_value);
    int get_health() const;

    void set_action(int p_value);
    int get_action() const;

    void set_flags(int p_value);
    int get_flags() const;

    void set_particle_flags(int p_value);
    int get_particle_flags() const;

    void set_invincibility_time(double p_value);
    double get_invincibility_time() const;

protected:
    static void _bind_methods();

private:
    godot::Vector3 position;
    godot::Vector3 velocity;
    float face_angle;
    int health;
    int action;
    int flags;
    int particle_flags;
    double invincibility_time;
};

#endif // LIBSM64GD_LIBSM64MARIOSTATE_H
