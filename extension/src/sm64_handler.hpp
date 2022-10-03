#ifndef LIBSM64GD_SM64HANDLER_H
#define LIBSM64GD_SM64HANDLER_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/image.hpp>

#include <sm64_input.hpp>
#include <sm64_surface_properties.hpp>

extern "C"
{
#include <libsm64.h>
}

#ifdef WIN32
#include <windows.h>
#endif

class SM64Handler : public godot::Resource
{
    GDCLASS(SM64Handler, godot::Resource);

public:
    SM64Handler();
    ~SM64Handler();

    void global_init();
    bool is_init() const;

    godot::Ref<godot::Image> get_mario_image() const;

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
    godot::Dictionary mario_tick(int mario_id, godot::Ref<SM64Input> input);

    void mario_delete(int mario_id);

    // void sm64_set_mario_action(int32_t marioId, uint32_t action);
    // void sm64_set_mario_state(int32_t marioId, uint32_t flags);
    void set_mario_position(int mario_id, godot::Vector3 position);
    void set_mario_angle(int mario_id, real_t face_angle);
    void set_mario_velocity(int mario_id, godot::Vector3 velocity);
    void set_mario_forward_velocity(int mario_id, real_t velocity);
    void set_mario_invincibility(int mario_id, int timer);
    void set_mario_water_level(int mario_id, real_t level);
    // void sm64_set_mario_floor_override(int32_t marioId, uint16_t terrain, int16_t floorType, int16_t floorForce);
    void set_mario_health(int mario_id, int health);
    void mario_take_damage(int mario_id, int damage, godot::Vector3 source_position, int subtype = 0);
    void mario_heal(int mario_id, int heal_counter);
    // void sm64_mario_set_lives(int32_t marioId, int32_t lives);
    // void sm64_mario_interact_cap( int32_t marioId, uint32_t capFlag, uint16_t capTime, uint8_t playMusic );
    // void sm64_mario_extend_cap( int32_t marioId, uint16_t capTime );
    // bool sm64_mario_attack(int32_t marioId, float x, float y, float z, float hitboxHeight);

    int surface_object_create(godot::PackedVector3Array vertexes, godot::Vector3 position, godot::Vector3 rotation, godot::TypedArray<SM64SurfaceProperties> suface_properties_array);
    void surface_object_move(int object_id, godot::Vector3 position, godot::Vector3 rotation);
    void surface_object_delete(int object_id);

protected:
    static void _bind_methods();

private:
    bool init = false;
    godot::Ref<godot::Image> mario_image;
    godot::String rom_filename;
    real_t scale_factor = 50.0;

    struct SM64MarioGeometryBuffers mario_geometry = {NULL, NULL, NULL, NULL, 0};

    godot::PackedVector3Array mario_position;
    godot::PackedVector3Array mario_normal;
    godot::PackedColorArray mario_color;
    godot::PackedVector2Array mario_uv;
};

#endif // LIBSM64GD_SM64HANDLER_H