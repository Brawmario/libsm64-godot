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

void SM64Audio::play_music(MusicID p_music_id, bool p_variant)
{
    uint16_t seq_arg = static_cast<uint16_t>(p_music_id) | (p_variant ? SEQ_VARIATION : 0);
    // seq_arg |= p_priority << 8;

    sm64_play_music(SEQ_PLAYER_LEVEL, seq_arg, 0);
}

void SM64Audio::stop_background_music(MusicID p_music_id)
{
    sm64_stop_background_music(static_cast<uint16_t>(p_music_id));
}

void SM64Audio::stop_current_background_music()
{
    sm64_stop_background_music(sm64_get_current_background_music());
}

SM64Audio::MusicID SM64Audio::get_current_background_music()
{
    int id = sm64_get_current_background_music();
    if (id < 0)
        return MUSIC_ID_COUNT;
    else
        return static_cast<MusicID>(id & 0xFF);
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
    godot::ClassDB::bind_method(godot::D_METHOD("play_music", "music_id", "variant"), &SM64Audio::play_music, DEFVAL(false));
    godot::ClassDB::bind_method(godot::D_METHOD("stop_background_music", "music_id"), &SM64Audio::stop_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_current_background_music"), &SM64Audio::stop_current_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("get_current_background_music"), &SM64Audio::get_current_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("set_volume", "volume"), &SM64Audio::set_volume);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_reverb", "reverb"), &SM64Global::set_reverb);

    BIND_ENUM_CONSTANT(PLAYER_LEVEL);
    BIND_ENUM_CONSTANT(PLAYER_ENV);
    BIND_ENUM_CONSTANT(PLAYER_SFX);

    BIND_ENUM_CONSTANT(MUSIC_ID_SOUND_PLAYER);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_COLLECT_STAR);
    BIND_ENUM_CONSTANT(MUSIC_ID_MENU_TITLE_SCREEN);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_GRASS);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_INSIDE_CASTLE);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_WATER);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_HOT);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_BOSS_KOOPA);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_SNOW);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_SLIDE);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_SPOOKY);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_PIRANHA_PLANT);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_UNDERGROUND);
    BIND_ENUM_CONSTANT(MUSIC_ID_MENU_STAR_SELECT);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_POWERUP);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_METAL_CAP);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_KOOPA_MESSAGE);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_KOOPA_ROAD);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_HIGH_SCORE);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_MERRY_GO_ROUND);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_RACE);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_STAR_SPAWN);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_BOSS);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_COLLECT_KEY);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_ENDLESS_STAIRS);
    BIND_ENUM_CONSTANT(MUSIC_ID_LEVEL_BOSS_KOOPA_FINAL);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_CREDITS);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_SOLVE_PUZZLE);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_TOAD_MESSAGE);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_PEACH_MESSAGE);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_INTRO);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_VICTORY);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_ENDING);
    BIND_ENUM_CONSTANT(MUSIC_ID_MENU_FILE_SELECT);
    BIND_ENUM_CONSTANT(MUSIC_ID_EVENT_CUTSCENE_LAKITU);
    BIND_ENUM_CONSTANT(MUSIC_ID_COUNT);
}
