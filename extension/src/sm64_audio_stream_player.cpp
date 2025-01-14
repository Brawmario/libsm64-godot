#include <sm64_audio_stream_player.hpp>

#include <godot_cpp/classes/engine.hpp>

#include <sm64_global.hpp>

#include <libsm64.h>

constexpr double g_sm64_delta = 1.0 / 30.0;
constexpr size_t g_sm64_audio_buffer_length = 544 * 2 * 2;

void SM64AudioStreamPlayer::audio_tick()
{
    godot::Ref<godot::AudioStreamGeneratorPlayback> playback = get_stream_playback();
    if (playback.is_null())
        return;

    const int buffered_frames = m_playback_frames_buffer_length - playback->get_frames_available();
    const int buffered_samples = buffered_frames / 2;

    std::array<int16_t, g_sm64_audio_buffer_length> audio_buffer;
    const int num_audio_samples = sm64_audio_tick(buffered_samples, 1100, audio_buffer.data());

    if (buffered_samples > 6000)
        return;

    const int frame_count = num_audio_samples * 2;
    m_frames.resize(frame_count);

    godot::Vector2 *frames_ptrw = m_frames.ptrw();
    for (int i = 0; i < frame_count; i++)
    {
        auto &frame = frames_ptrw[i];
        auto *audio_buffer_frame = &audio_buffer[2 * i];
        frame.x = real_t(audio_buffer_frame[0]) / 32767.0;
        frame.y = real_t(audio_buffer_frame[1]) / 32767.0;
    }

    playback->push_buffer(m_frames);
}

SM64AudioStreamPlayer::SM64AudioStreamPlayer()
{
    m_time_since_last_tick = 0.0;
}

void SM64AudioStreamPlayer::_ready()
{
    if (godot::Engine::get_singleton()->is_editor_hint())
        return;

    godot::Ref<godot::AudioStreamGenerator> stream;
    stream.instantiate();
    stream->set_mix_rate(32000.0);
    set_stream(stream);

    // Playback buffer size math since Godot 4.3
    const int target_frames_buffer_length = stream->get_mix_rate() * stream->get_buffer_length();
    m_playback_frames_buffer_length = godot::next_power_of_2(target_frames_buffer_length);
}

void SM64AudioStreamPlayer::_process(double delta)
{
    if (godot::Engine::get_singleton()->is_editor_hint())
        return;

    if (!is_playing())
        return;

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);

    m_time_since_last_tick += delta;
    while (m_time_since_last_tick >= g_sm64_delta)
    {
        if (is_playing() && sm64_global->is_init())
            audio_tick();
        m_time_since_last_tick -= g_sm64_delta;
    }
}


void SM64AudioStreamPlayer::play_music(int p_seq_id, bool p_variant)
{
    uint16_t seq_arg = static_cast<uint16_t>(p_seq_id) | (p_variant ? 0x80 : 0);
    // seq_arg |= p_priority << 8;

    sm64_play_music(0, seq_arg, 0);
}

void SM64AudioStreamPlayer::stop_background_music(int p_seq_id)
{
    sm64_stop_background_music(static_cast<uint16_t>(p_seq_id));
}

void SM64AudioStreamPlayer::stop_current_background_music()
{
    sm64_stop_background_music(sm64_get_current_background_music());
}

int SM64AudioStreamPlayer::get_current_background_music()
{
    return sm64_get_current_background_music();
}

void SM64AudioStreamPlayer::set_internal_volume(real_t p_volume)
{
    sm64_set_sound_volume(p_volume);
}

void SM64AudioStreamPlayer::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("play_music", "seq_id", "variant"), &SM64AudioStreamPlayer::play_music, DEFVAL(false));
    godot::ClassDB::bind_method(godot::D_METHOD("stop_background_music", "seq_id"), &SM64AudioStreamPlayer::stop_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_current_background_music"), &SM64AudioStreamPlayer::stop_current_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("get_current_background_music"), &SM64AudioStreamPlayer::get_current_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("set_internal_volume", "volume"), &SM64AudioStreamPlayer::set_internal_volume);
}
