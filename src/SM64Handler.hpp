#include <Godot.hpp>
#include <Reference.hpp>
#include<Image.hpp>

extern "C"
{
#include <libsm64.h>
}

class SM64Handler : public godot::Reference
{
    GODOT_CLASS(SM64Handler, godot::Reference);

public:
    SM64Handler();
    ~SM64Handler();

    void _init();

    godot::Ref<godot::Image> global_init(godot::String rom_filename);
    void static_surfaces_load(godot::PoolVector3Array vertexes);

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

    // TODO: implement the following
    // uint32_t sm64_surface_object_create(const struct SM64SurfaceObject *surfaceObject);
    // void sm64_surface_object_move(uint32_t objectId, const struct SM64ObjectTransform *transform);
    // void sm64_surface_object_delete(uint32_t objectId);

    static void _register_methods();

private:
    struct SM64MarioGeometryBuffers mario_geometry = {NULL, NULL, NULL, NULL, 0};

    godot::PoolVector3Array mario_position;
    godot::PoolVector3Array mario_normal;
    godot::PoolColorArray mario_color;
    godot::PoolVector2Array mario_uv;
};
