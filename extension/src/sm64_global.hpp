#ifndef LIBSM64GD_SM64GLOBAL_H
#define LIBSM64GD_SM64GLOBAL_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>

extern "C"
{
#include <libsm64.h>
}

#ifdef WIN32
#include <windows.h>
#endif

class SM64Global : public godot::Object
{
    GDCLASS(SM64Global, godot::Object);

    inline static SM64Global *singleton = nullptr;

public:
    SM64Global();
    ~SM64Global();

    static SM64Global *get_singleton();

    void init();
    void terminate();
    bool is_init() const;

    godot::Ref<godot::Image> get_mario_image();
    godot::Ref<godot::ImageTexture> get_mario_image_texture();

    void set_rom_filepath(const godot::String &p_value);
    godot::String get_rom_filepath() const;

    void set_scale_factor(real_t p_value);
    real_t get_scale_factor() const;

    void set_volume(real_t p_volume);
    // void set_reverb(int p_reverb);

protected:
    static void _bind_methods();

private:

    godot::String m_rom_filepath = "";
    real_t m_scale_factor = 75.0;

    bool m_init = false;
    godot::Ref<godot::Image> m_mario_image = nullptr;
    godot::Ref<godot::ImageTexture> m_mario_image_texture = nullptr;
};

#endif // LIBSM64GD_SM64GLOBAL_H