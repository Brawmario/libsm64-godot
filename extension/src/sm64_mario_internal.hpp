#ifndef LIBSM64GD_SM64MARIOINTERNAL_H
#define LIBSM64GD_SM64MARIOINTERNAL_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

#include <sm64_surface_properties.hpp>
#include <sm64_mario_geometry.hpp>

#include <libsm64.h>

#ifdef WIN32
#include <windows.h>
#endif

class SM64MarioInternal : public godot::RefCounted
{
    GDCLASS(SM64MarioInternal, godot::RefCounted);

public:
    SM64MarioInternal() = default;

    void set_interpolate(bool value);
    bool get_interpolate() const;

    void reset_interpolation();

    int mario_create(godot::Vector3 p_position, godot::Vector3 p_rotation);

    /**
     * @brief Tick Mario with the provided ID
     *
     * @param mario_id Ticked Mario's ID.
     * @param input Mario's inputs. format:
     * {
     *     "cam_look": godot::Vector2 (Camera's X and Z coordinates in Godot),
     *     "stick":    godot::Vector2,
     *     "a":        bool,
     *     "b":        bool,
     *     "z":        bool,
     * }
     * @return godot::Dictionary Mario's state and MeshArray. format:
     * {
     *     "position": godot::Vector3,
     *     "velocity": godot::Vector3,
     *     "face_angle": real_t,
     *     "health": int,
     *     "array_mesh": godot::Ref<godot::ArrayMesh>,
     * }
     */
    godot::Dictionary tick(real_t delta, godot::Dictionary p_input);

    void mario_delete();

    void set_action(int p_action);
    void set_state(int p_flags);
    void set_position(godot::Vector3 p_position);
    void set_angle(godot::Vector3 p_rotation);
    void set_face_angle(real_t p_value);
    void set_velocity(godot::Vector3 p_velocity);
    void set_forward_velocity(real_t p_velocity);
    void set_invincibility(real_t p_time);
    void set_water_level(real_t p_level);
    void set_gas_level(real_t p_level);
    // void set_floor_override(godot::Ref<SM64SurfaceProperties> p_surface_properties);
    // void reset_floor_override();
    void set_health(int p_health);
    void take_damage(int p_damage, godot::Vector3 p_source_position, bool p_big_knockback = false);
    void heal(int p_wedges);
    void kill();
    // void set_lives(int p_lives);
    void interact_cap(int p_cap, real_t p_cap_time = 0.0, bool p_play_music = true);
    void extend_cap(real_t p_cap_time);
    // bool sm64_mario_attack(int32_t marioId, float x, float y, float z, float hitboxHeight);

protected:
    static void _bind_methods();

private:
    int m_id = -1;
    bool m_interpolate = true;

    bool m_first_tick = true;
    real_t m_time_since_last_tick = 0.0;

    int m_current_index = 0;
    int m_last_index = 1;

    bool m_reset_interpolation_next_tick = false;

    struct SM64MarioState m_out_state;
    std::array<struct SM64MarioState, 2> m_out_state_hard_tick;

    SM64MarioGeometry m_geometry;
    std::array<SM64MarioGeometry, 2> m_geometry_hard_tick;

    godot::PackedVector3Array m_position;
    godot::PackedVector3Array m_normal;
    godot::PackedColorArray m_color;
    godot::PackedVector2Array m_uv;
};

#endif // LIBSM64GD_SM64MARIOINTERNAL_H
