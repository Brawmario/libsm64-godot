#include <Godot.hpp>
#include <Reference.hpp>

class SM64Handler : public godot::Reference
{
    GODOT_CLASS(SM64Handler, godot::Reference);

public:
    SM64Handler();
    ~SM64Handler();

    void _init();

    void global_init(godot::String rom_filename);
    void static_surfaces_load(godot::PoolVector3Array vertexes);

    static void _register_methods();

private:
    uint8_t *mario_texture = NULL;
};

/*
    void sm64_static_surfaces_load(const struct SM64Surface *surfaceArray, uint32_t numSurfaces);

    int32_t sm64_mario_create(int16_t x, int16_t y, int16_t z);
    void sm64_mario_tick(int32_t marioId, const struct SM64MarioInputs *inputs, struct SM64MarioState *outState, struct SM64MarioGeometryBuffers *outBuffers);
    void sm64_mario_delete(int32_t marioId);

    uint32_t sm64_surface_object_create(const struct SM64SurfaceObject *surfaceObject);
    void sm64_surface_object_move(uint32_t objectId, const struct SM64ObjectTransform *transform);
    void sm64_surface_object_delete(uint32_t objectId);
*/