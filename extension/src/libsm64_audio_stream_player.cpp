#include <libsm64_audio_stream_player.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/audio_stream_generator.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>

#include <libsm64.hpp>

void LibSM64AudioStreamPlayer::audio_tick()
{
    godot::Ref<godot::AudioStreamGeneratorPlayback> playback = get_stream_playback();
    ERR_FAIL_NULL(playback);

    const auto buffered_frames = playback_frames_buffer_length - playback->get_frames_available();

    LibSM64 *libsm64 = LibSM64::get_singleton();
    ERR_FAIL_NULL(libsm64);

    auto audio_frames = libsm64->audio_tick(buffered_frames, 2200);

    if (buffered_frames > 12000)
        return;

    playback->push_buffer(audio_frames);
}

LibSM64AudioStreamPlayer::LibSM64AudioStreamPlayer()
{
    time_since_last_tick = 0.0;
}

void LibSM64AudioStreamPlayer::_ready()
{
    if (godot::Engine::get_singleton()->is_editor_hint())
        return;

    godot::Ref<godot::AudioStreamGenerator> stream;
    stream.instantiate();
    stream->set_mix_rate(32000.0);
    set_stream(stream);

    // Playback buffer size math since Godot 4.3
    const int target_frames_buffer_length = stream->get_mix_rate() * stream->get_buffer_length();
    playback_frames_buffer_length = godot::next_power_of_2(target_frames_buffer_length);
}

void LibSM64AudioStreamPlayer::_process(double delta)
{
    if (godot::Engine::get_singleton()->is_editor_hint())
        return;

    if (!is_playing())
        return;

    time_since_last_tick += delta;
    while (time_since_last_tick >= g_sm64_timestep_interval)
    {
        audio_tick();
        time_since_last_tick -= g_sm64_timestep_interval;
    }
}
