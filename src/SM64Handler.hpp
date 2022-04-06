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

    static void _register_methods();

private:
    uint8_t *mario_texture = NULL;
};
