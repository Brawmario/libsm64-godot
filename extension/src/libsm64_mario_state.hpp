#ifndef LIBSM64GD_LIBSM64MARIOSTATE_H
#define LIBSM64GD_LIBSM64MARIOSTATE_H

#include <godot_cpp/classes/ref.hpp>

#include <libsm64.hpp>

#ifdef WIN32
#include <windows.h>
#endif

class LibSM64MarioState : public godot::RefCounted
{
    GDCLASS(LibSM64MarioState, godot::RefCounted);

    friend class LibSM64MarioInterpolator;

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

    void set_action(godot::BitField<LibSM64::ActionFlags> p_value);
    godot::BitField<LibSM64::ActionFlags> get_action() const;

    void set_flags(godot::BitField<LibSM64::MarioFlags> p_value);
    godot::BitField<LibSM64::MarioFlags> get_flags() const;

    void set_particle_flags(godot::BitField<LibSM64::ParticleFlags> p_value);
    godot::BitField<LibSM64::ParticleFlags> get_particle_flags() const;

    void set_invincibility_time(double p_value);
    double get_invincibility_time() const;

protected:
    static void _bind_methods();

private:
    godot::Vector3 position;
    godot::Vector3 velocity;
    float face_angle;
    int health;
    godot::BitField<LibSM64::ActionFlags> action;
    godot::BitField<LibSM64::MarioFlags> flags;
    godot::BitField<LibSM64::ParticleFlags> particle_flags;
    double invincibility_time;
};

#endif // LIBSM64GD_LIBSM64MARIOSTATE_H
