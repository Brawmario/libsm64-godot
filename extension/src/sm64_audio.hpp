#ifndef LIBSM64GD_SM64AUDIO_H
#define LIBSM64GD_SM64AUDIO_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>

#include <libsm64.h>
#include <libsm64/src/decomp/include/seq_ids.h>

#ifdef WIN32
#include <windows.h>
#endif

#define SM64_SEQ_PLAYER_LEVEL            0  // Level background music
#define SM64_SEQ_PLAYER_ENV              1  // Misc music like the puzzle jingle
#define SM64_SEQ_PLAYER_SFX              2  // Sound effects

class SM64Audio : public godot::Object
{
    GDCLASS(SM64Audio, godot::Object);

    inline static SM64Audio *singleton = nullptr;

public:
    SM64Audio();
    ~SM64Audio();

    static SM64Audio *get_singleton();

    enum Player {
        PLAYER_LEVEL = SM64_SEQ_PLAYER_LEVEL, // Level background music
        PLAYER_ENV   = SM64_SEQ_PLAYER_ENV,   // Misc music like the puzzle jingle
        PLAYER_SFX   = SM64_SEQ_PLAYER_SFX,   // Sound effects
    };

    enum MusicID {
        MUSIC_ID_SOUND_PLAYER                = SEQ_SOUND_PLAYER,                 // 0x00
        MUSIC_ID_EVENT_CUTSCENE_COLLECT_STAR = SEQ_EVENT_CUTSCENE_COLLECT_STAR,  // 0x01
        MUSIC_ID_MENU_TITLE_SCREEN           = SEQ_MENU_TITLE_SCREEN,            // 0x02
        MUSIC_ID_LEVEL_GRASS                 = SEQ_LEVEL_GRASS,                  // 0x03
        MUSIC_ID_LEVEL_INSIDE_CASTLE         = SEQ_LEVEL_INSIDE_CASTLE,          // 0x04
        MUSIC_ID_LEVEL_WATER                 = SEQ_LEVEL_WATER,                  // 0x05
        MUSIC_ID_LEVEL_HOT                   = SEQ_LEVEL_HOT,                    // 0x06
        MUSIC_ID_LEVEL_BOSS_KOOPA            = SEQ_LEVEL_BOSS_KOOPA,             // 0x07
        MUSIC_ID_LEVEL_SNOW                  = SEQ_LEVEL_SNOW,                   // 0x08
        MUSIC_ID_LEVEL_SLIDE                 = SEQ_LEVEL_SLIDE,                  // 0x09
        MUSIC_ID_LEVEL_SPOOKY                = SEQ_LEVEL_SPOOKY,                 // 0x0A
        MUSIC_ID_EVENT_PIRANHA_PLANT         = SEQ_EVENT_PIRANHA_PLANT,          // 0x0B
        MUSIC_ID_LEVEL_UNDERGROUND           = SEQ_LEVEL_UNDERGROUND,            // 0x0C
        MUSIC_ID_MENU_STAR_SELECT            = SEQ_MENU_STAR_SELECT,             // 0x0D
        MUSIC_ID_EVENT_POWERUP               = SEQ_EVENT_POWERUP,                // 0x0E
        MUSIC_ID_EVENT_METAL_CAP             = SEQ_EVENT_METAL_CAP,              // 0x0F
        MUSIC_ID_EVENT_KOOPA_MESSAGE         = SEQ_EVENT_KOOPA_MESSAGE,          // 0x10
        MUSIC_ID_LEVEL_KOOPA_ROAD            = SEQ_LEVEL_KOOPA_ROAD,             // 0x11
        MUSIC_ID_EVENT_HIGH_SCORE            = SEQ_EVENT_HIGH_SCORE,             // 0x12
        MUSIC_ID_EVENT_MERRY_GO_ROUND        = SEQ_EVENT_MERRY_GO_ROUND,         // 0x13
        MUSIC_ID_EVENT_RACE                  = SEQ_EVENT_RACE,                   // 0x14
        MUSIC_ID_EVENT_CUTSCENE_STAR_SPAWN   = SEQ_EVENT_CUTSCENE_STAR_SPAWN,    // 0x15
        MUSIC_ID_EVENT_BOSS                  = SEQ_EVENT_BOSS,                   // 0x16
        MUSIC_ID_EVENT_CUTSCENE_COLLECT_KEY  = SEQ_EVENT_CUTSCENE_COLLECT_KEY,   // 0x17
        MUSIC_ID_EVENT_ENDLESS_STAIRS        = SEQ_EVENT_ENDLESS_STAIRS,         // 0x18
        MUSIC_ID_LEVEL_BOSS_KOOPA_FINAL      = SEQ_LEVEL_BOSS_KOOPA_FINAL,       // 0x19
        MUSIC_ID_EVENT_CUTSCENE_CREDITS      = SEQ_EVENT_CUTSCENE_CREDITS,       // 0x1A
        MUSIC_ID_EVENT_SOLVE_PUZZLE          = SEQ_EVENT_SOLVE_PUZZLE,           // 0x1B
        MUSIC_ID_EVENT_TOAD_MESSAGE          = SEQ_EVENT_TOAD_MESSAGE,           // 0x1C
        MUSIC_ID_EVENT_PEACH_MESSAGE         = SEQ_EVENT_PEACH_MESSAGE,          // 0x1D
        MUSIC_ID_EVENT_CUTSCENE_INTRO        = SEQ_EVENT_CUTSCENE_INTRO,         // 0x1E
        MUSIC_ID_EVENT_CUTSCENE_VICTORY      = SEQ_EVENT_CUTSCENE_VICTORY,       // 0x1F
        MUSIC_ID_EVENT_CUTSCENE_ENDING       = SEQ_EVENT_CUTSCENE_ENDING,        // 0x20
        MUSIC_ID_MENU_FILE_SELECT            = SEQ_MENU_FILE_SELECT,             // 0x21
        MUSIC_ID_EVENT_CUTSCENE_LAKITU       = SEQ_EVENT_CUTSCENE_LAKITU,        // 0x22 (not in JP)
        MUSIC_ID_COUNT                       = SEQ_COUNT
    };

    // void sm64_seq_player_play_sequence(uint8_t player, uint8_t seqId, uint16_t arg2);
    void play_music(MusicID p_music_id, bool p_variant = false);
    void stop_background_music(MusicID p_seq_id);
    void stop_current_background_music();
    // void sm64_fadeout_background_music(uint16_t arg0, uint16_t fadeOut);
    MusicID get_current_background_music();
    // void sm64_play_sound(int32_t soundBits, float *pos);
    // void sm64_play_sound_global(int32_t soundBits);
    // void sm64_set_sound_volume(float vol);

    void set_volume(real_t p_volume);
    // void set_reverb(int p_reverb);

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(SM64Audio::Player);
VARIANT_ENUM_CAST(SM64Audio::MusicID);

#endif // LIBSM64GD_SM64AUDIO_H
