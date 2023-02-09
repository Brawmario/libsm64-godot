#include <sm64_audio.hpp>

constexpr real_t g_sm64_delta = 1.0/30.0;

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

void SM64Audio::play_music(int p_player, int p_seq_args, real_t p_fade_time)
{
    sm64_play_music((uint8_t) p_player, (uint16_t) p_seq_args, (uint16_t) p_fade_time / g_sm64_delta);
}

void SM64Audio::stop_background_music(int p_seq_id)
{
    sm64_stop_background_music((uint16_t) p_seq_id);
}

void SM64Audio::stop_current_background_music()
{
    sm64_stop_background_music(sm64_get_current_background_music());
}

int SM64Audio::get_current_background_music()
{
    return sm64_get_current_background_music();
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
    godot::ClassDB::bind_method(godot::D_METHOD("play_music", "player", "seq_args", "fade_time"), &SM64Audio::play_music);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_background_music", "seq_id"), &SM64Audio::stop_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_current_background_music"), &SM64Audio::stop_current_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("get_current_background_music"), &SM64Audio::get_current_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("set_volume", "volume"), &SM64Audio::set_volume);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_reverb", "reverb"), &SM64Global::set_reverb);
}
