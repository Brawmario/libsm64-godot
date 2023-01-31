#ifndef LIBSM64GD_SM64HANDLER_H
#define LIBSM64GD_SM64HANDLER_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>

#include <sm64_input.hpp>
#include <sm64_surface_properties.hpp>

extern "C"
{
#include <libsm64.h>
}

#ifdef WIN32
#include <windows.h>
#endif

#define MARIO_NORMAL_CAP                0x00000001
#define MARIO_VANISH_CAP                0x00000002
#define MARIO_METAL_CAP                 0x00000004
#define MARIO_WING_CAP                  0x00000008

class LibSM64 : public godot::Object
{
    GDCLASS(LibSM64, godot::Object);

    inline static LibSM64 *singleton = nullptr;

public:
    LibSM64();
    ~LibSM64();

    static LibSM64 *get_singleton();

    enum MarioCaps {
        MARIO_CAPS_NORMAL = MARIO_NORMAL_CAP,
        MARIO_CAPS_VANISH = MARIO_VANISH_CAP,
        MARIO_CAPS_METAL = MARIO_METAL_CAP,
        MARIO_CAPS_WING = MARIO_WING_CAP
    };

    void global_init();
    void global_terminate();
    bool is_init() const;

    godot::Ref<godot::Image> get_mario_image();
    godot::Ref<godot::ImageTexture> get_mario_image_texture();

    void set_rom_filename(const godot::String &value);
    godot::String get_rom_filename() const;

    void set_scale_factor(real_t value);
    real_t get_scale_factor() const;

    void static_surfaces_load(godot::PackedVector3Array vertexes, godot::TypedArray<SM64SurfaceProperties> surface_properties_array);

    int mario_create(godot::Vector3 position, godot::Vector3 rotation);

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
    godot::Dictionary mario_tick(int mario_id, godot::Dictionary input);

    void mario_delete(int mario_id);

    void set_mario_action(int mario_id, int action);
    void set_mario_state(int mario_id, int flags);
    void set_mario_position(int mario_id, godot::Vector3 position);
    void set_mario_angle(int mario_id, real_t face_angle);
    void set_mario_velocity(int mario_id, godot::Vector3 velocity);
    void set_mario_forward_velocity(int mario_id, real_t velocity);
    void set_mario_invincibility(int mario_id, int timer);
    void set_mario_water_level(int mario_id, real_t level);
    void set_mario_floor_override(int mario_id, godot::Ref<SM64SurfaceProperties> surface_properties);
    void reset_mario_floor_override(int mario_id);
    void set_mario_health(int mario_id, int health);
    void mario_take_damage(int mario_id, int damage, godot::Vector3 source_position, bool big_knockback = false);
    void mario_heal(int mario_id, int heal_counter);
    void mario_set_lives(int mario_id, int lives);
    void mario_interact_cap(int mario_id, MarioCaps cap, int cap_time = 0, bool play_music = 1);
    void mario_extend_cap(int mario_id, int cap_time);
    // bool sm64_mario_attack(int32_t marioId, float x, float y, float z, float hitboxHeight);

    int surface_object_create(godot::PackedVector3Array vertexes, godot::Vector3 position, godot::Vector3 rotation, godot::TypedArray<SM64SurfaceProperties> suface_properties_array);
    void surface_object_move(int object_id, godot::Vector3 position, godot::Vector3 rotation);
    void surface_object_delete(int object_id);

    void set_volume(real_t volume);
    void set_reverb(int reverb);

protected:
    static void _bind_methods();

private:
    bool init = false;
    godot::Ref<godot::Image> mario_image = nullptr;
    godot::Ref<godot::ImageTexture> mario_image_texture = nullptr;
    godot::String rom_filename = "";
    real_t scale_factor = 75.0;

    struct SM64MarioGeometryBuffers mario_geometry = {NULL, NULL, NULL, NULL, 0};

    godot::PackedVector3Array mario_position;
    godot::PackedVector3Array mario_normal;
    godot::PackedColorArray mario_color;
    godot::PackedVector2Array mario_uv;
};

VARIANT_ENUM_CAST(LibSM64, MarioCaps);

#endif // LIBSM64GD_SM64HANDLER_H