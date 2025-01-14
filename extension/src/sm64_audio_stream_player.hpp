#ifndef LIBSM64GD_SM64AUDIOSTREAMPLAYER_H
#define LIBSM64GD_SM64AUDIOSTREAMPLAYER_H

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_generator.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>

class SM64AudioStreamPlayer : public godot::AudioStreamPlayer
{
    GDCLASS(SM64AudioStreamPlayer, godot::AudioStreamPlayer);

private:
    double m_time_since_last_tick;
    godot::PackedVector2Array m_frames;
    int m_playback_frames_buffer_length;

    void audio_tick();

public:
    SM64AudioStreamPlayer();

    virtual void _ready() override;
    virtual void _process(double delta) override;

    // void sm64_seq_player_play_sequence(uint8_t player, uint8_t seqId, uint16_t arg2);
    void play_music(int p_seq_id, bool p_variant = false);
    void stop_background_music(int p_seq_id);
    void stop_current_background_music();
    // void sm64_fadeout_background_music(uint16_t arg0, uint16_t fadeOut);
    int get_current_background_music();
    // void sm64_play_sound(int32_t soundBits, float *pos);
    // void sm64_play_sound_global(int32_t soundBits);
    // void sm64_set_sound_volume(float vol);

    void set_internal_volume(real_t p_volume);

protected:
    static void _bind_methods();
};

#endif // LIBSM64GD_SM64AUDIOSTREAMPLAYER_H
