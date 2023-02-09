#ifndef LIBSM64GD_SM64AUDIO_H
#define LIBSM64GD_SM64AUDIO_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>

#include <libsm64.h>

#ifdef WIN32
#include <windows.h>
#endif

class SM64Audio : public godot::Object
{
    GDCLASS(SM64Audio, godot::Object);

    inline static SM64Audio *singleton = nullptr;

public:
    SM64Audio();
    ~SM64Audio();

    static SM64Audio *get_singleton();

    // void sm64_seq_player_play_sequence(uint8_t player, uint8_t seqId, uint16_t arg2);
    void play_music(int p_player, int p_seq_args, real_t p_fade_time);
    void stop_background_music(int p_seq_id);
    void stop_current_background_music();
    // void sm64_fadeout_background_music(uint16_t arg0, uint16_t fadeOut);
    int get_current_background_music();
    // void sm64_play_sound(int32_t soundBits, float *pos);
    // void sm64_play_sound_global(int32_t soundBits);
    // void sm64_set_sound_volume(float vol);

    void set_volume(real_t p_volume);
    // void set_reverb(int p_reverb);

protected:
    static void _bind_methods();
};

#endif // LIBSM64GD_SM64AUDIO_H