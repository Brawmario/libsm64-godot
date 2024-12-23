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

    auto audio_buffer_it = audio_buffer.begin();
    for (auto &frame : m_frames)
    {
        frame.x = real_t(*(audio_buffer_it++)) / 32767.0;
        frame.y = real_t(*(audio_buffer_it++)) / 32767.0;
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


void SM64AudioStreamPlayer::play_music(SeqID p_seq_id, bool p_variant)
{
    uint16_t seq_arg = static_cast<uint16_t>(p_seq_id) | (p_variant ? SEQ_VARIATION : 0);
    // seq_arg |= p_priority << 8;

    sm64_play_music(SM64_SEQ_PLAYER_LEVEL, seq_arg, 0);
}

void SM64AudioStreamPlayer::stop_background_music(SeqID p_seq_id)
{
    sm64_stop_background_music(static_cast<uint16_t>(p_seq_id));
}

void SM64AudioStreamPlayer::stop_current_background_music()
{
    sm64_stop_background_music(sm64_get_current_background_music());
}

SM64AudioStreamPlayer::SeqID SM64AudioStreamPlayer::get_current_background_music()
{
    int id = sm64_get_current_background_music();
    if (id < 0)
        return SEQ_ID_COUNT;
    else
        return static_cast<SeqID>(id & 0xFF);
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

    BIND_ENUM_CONSTANT(SEQ_PLAYER_LEVEL);
    BIND_ENUM_CONSTANT(SEQ_PLAYER_ENV);
    BIND_ENUM_CONSTANT(SEQ_PLAYER_SFX);

    BIND_ENUM_CONSTANT(SEQ_ID_SOUND_PLAYER);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_COLLECT_STAR);
    BIND_ENUM_CONSTANT(SEQ_ID_MENU_TITLE_SCREEN);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_GRASS);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_INSIDE_CASTLE);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_WATER);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_HOT);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_BOSS_KOOPA);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_SNOW);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_SLIDE);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_SPOOKY);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_PIRANHA_PLANT);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_UNDERGROUND);
    BIND_ENUM_CONSTANT(SEQ_ID_MENU_STAR_SELECT);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_POWERUP);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_METAL_CAP);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_KOOPA_MESSAGE);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_KOOPA_ROAD);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_HIGH_SCORE);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_MERRY_GO_ROUND);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_RACE);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_STAR_SPAWN);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_BOSS);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_COLLECT_KEY);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_ENDLESS_STAIRS);
    BIND_ENUM_CONSTANT(SEQ_ID_LEVEL_BOSS_KOOPA_FINAL);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_CREDITS);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_SOLVE_PUZZLE);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_TOAD_MESSAGE);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_PEACH_MESSAGE);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_INTRO);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_VICTORY);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_ENDING);
    BIND_ENUM_CONSTANT(SEQ_ID_MENU_FILE_SELECT);
    BIND_ENUM_CONSTANT(SEQ_ID_EVENT_CUTSCENE_LAKITU);
    BIND_ENUM_CONSTANT(SEQ_ID_COUNT);
}
