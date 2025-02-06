#ifndef LIBSM64GD_LIBSM64MARIOSTATE_H
#define LIBSM64GD_LIBSM64MARIOSTATE_H

#include <godot_cpp/classes/ref.hpp>

#include <libsm64.hpp>

class LibSM64MarioState : public godot::RefCounted {
	GDCLASS(LibSM64MarioState, godot::RefCounted);

	friend class LibSM64MarioInterpolator;

public:
	LibSM64MarioState() = default;
	LibSM64MarioState(const struct SM64MarioState &state, real_t scale_factor);

	void set_position(const godot::Vector3 &p_value);
	godot::Vector3 get_position() const;

	void set_velocity(const godot::Vector3 &p_value);
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
	float face_angle = 0.0f;
	int health = 0;
	godot::BitField<LibSM64::ActionFlags> action = 0;
	godot::BitField<LibSM64::MarioFlags> flags = 0;
	godot::BitField<LibSM64::ParticleFlags> particle_flags = 0;
	double invincibility_time = 0.0;
};

#endif // LIBSM64GD_LIBSM64MARIOSTATE_H
