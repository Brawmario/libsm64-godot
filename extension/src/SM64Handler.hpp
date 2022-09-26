#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/image.hpp>


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
    void static_surfaces_load(godot::PackedVector3Array vertexes);

    int mario_create(godot::Vector3 vec);

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
    godot::Dictionary mario_tick(int mario_id, godot::Dictionary inputs);

    void mario_delete(int mario_id);

    int surface_object_create(godot::PackedVector3Array vertexes, godot::Vector3 position, godot::Vector3 rotation);
    void surface_object_move(int object_id, godot::Vector3 position, godot::Vector3 rotation);
    void surface_object_delete(int object_id);

protected:
    static void _bind_methods();

private:
    bool is_init = false;
    godot::Ref<godot::Image> mario_image;
    godot::String rom_filename;
    real_t scale_factor;

    struct SM64MarioGeometryBuffers mario_geometry = {NULL, NULL, NULL, NULL, 0};

    godot::PackedVector3Array mario_position;
    godot::PackedVector3Array mario_normal;
    godot::PackedColorArray mario_color;
    godot::PackedVector2Array mario_uv;
};
