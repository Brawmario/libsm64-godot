#include <sm64_audio.hpp>

SM64Audio::SM64Audio()
{
    ERR_FAIL_COND(singleton != nullptr);
    singleton = this;
}

SM64Audio::~SM64Audio()
{
    ERR_FAIL_COND(singleton != this);
    singleton = nullptr;
}

SM64Audio *SM64Audio::get_singleton()
{
    return singleton;
}

void SM64Audio::set_volume(real_t p_volume)
{
    sm64_set_sound_volume(p_volume);
}

// void SM64Audio::set_reverb(int p_reverb)
// {
//     sm64_set_reverb((uint8_t) p_reverb);
// }

void SM64Audio::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_volume", "volume"), &SM64Audio::set_volume);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_reverb", "reverb"), &SM64Global::set_reverb);
}
