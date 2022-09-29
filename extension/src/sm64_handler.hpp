#ifndef LIBSM64GD_SM64HANDLER_H
#define LIBSM64GD_SM64HANDLER_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref.hpp>

#include <sm64_input.hpp>

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


    void static_surfaces_load(godot::PackedVector3Array vertexes);

    int mario_create(godot::Vector3 position);

    /**
     * @brief Tick Mario with the provided ID
     * 
     * @param mario_id Ticked Mario's ID.
     * @param inputs Mario's inputs. format:
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

    int surface_object_create(godot::PackedVector3Array vertexes, godot::Vector3 position, godot::Vector3 rotation);
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