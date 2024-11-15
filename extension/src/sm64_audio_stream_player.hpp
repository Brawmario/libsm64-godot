#ifndef LIBSM64GD_SM64AUDIOSTREAMPLAYER_H
#define LIBSM64GD_SM64AUDIOSTREAMPLAYER_H

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_generator.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>

#include <libsm64/src/decomp/include/seq_ids.h>

#define SM64_SEQ_PLAYER_LEVEL            0  // Level background music
#define SM64_SEQ_PLAYER_ENV              1  // Misc music like the puzzle jingle
#define SM64_SEQ_PLAYER_SFX              2  // Sound effects

using namespace godot;

class SM64AudioStreamPlayer : public AudioStreamPlayer
{
    GDCLASS(SM64AudioStreamPlayer, AudioStreamPlayer);

private:
    real_t m_time_since_last_tick;
    PackedVector2Array m_frames;
    int m_max_queued_frames;

    Ref<AudioStreamGenerator> m_stream;
    Ref<AudioStreamGeneratorPlayback> m_playback;

    void audio_tick();

public:
    SM64AudioStreamPlayer();

    virtual void _ready() override;
    virtual void _process(double delta) override;

    enum SeqPlayer {
        SEQ_PLAYER_LEVEL = SM64_SEQ_PLAYER_LEVEL, // Level background music
        SEQ_PLAYER_ENV   = SM64_SEQ_PLAYER_ENV,   // Misc music like the puzzle jingle
        SEQ_PLAYER_SFX   = SM64_SEQ_PLAYER_SFX,   // Sound effects
    };

    enum SeqID {
        SEQ_ID_SOUND_PLAYER                = SEQ_SOUND_PLAYER,                 // 0x00
        SEQ_ID_EVENT_CUTSCENE_COLLECT_STAR = SEQ_EVENT_CUTSCENE_COLLECT_STAR,  // 0x01
        SEQ_ID_MENU_TITLE_SCREEN           = SEQ_MENU_TITLE_SCREEN,            // 0x02
        SEQ_ID_LEVEL_GRASS                 = SEQ_LEVEL_GRASS,                  // 0x03
        SEQ_ID_LEVEL_INSIDE_CASTLE         = SEQ_LEVEL_INSIDE_CASTLE,          // 0x04
        SEQ_ID_LEVEL_WATER                 = SEQ_LEVEL_WATER,                  // 0x05
        SEQ_ID_LEVEL_HOT                   = SEQ_LEVEL_HOT,                    // 0x06
        SEQ_ID_LEVEL_BOSS_KOOPA            = SEQ_LEVEL_BOSS_KOOPA,             // 0x07
        SEQ_ID_LEVEL_SNOW                  = SEQ_LEVEL_SNOW,                   // 0x08
        SEQ_ID_LEVEL_SLIDE                 = SEQ_LEVEL_SLIDE,                  // 0x09
        SEQ_ID_LEVEL_SPOOKY                = SEQ_LEVEL_SPOOKY,                 // 0x0A
        SEQ_ID_EVENT_PIRANHA_PLANT         = SEQ_EVENT_PIRANHA_PLANT,          // 0x0B
        SEQ_ID_LEVEL_UNDERGROUND           = SEQ_LEVEL_UNDERGROUND,            // 0x0C
        SEQ_ID_MENU_STAR_SELECT            = SEQ_MENU_STAR_SELECT,             // 0x0D
        SEQ_ID_EVENT_POWERUP               = SEQ_EVENT_POWERUP,                // 0x0E
        SEQ_ID_EVENT_METAL_CAP             = SEQ_EVENT_METAL_CAP,              // 0x0F
        SEQ_ID_EVENT_KOOPA_MESSAGE         = SEQ_EVENT_KOOPA_MESSAGE,          // 0x10
        SEQ_ID_LEVEL_KOOPA_ROAD            = SEQ_LEVEL_KOOPA_ROAD,             // 0x11
        SEQ_ID_EVENT_HIGH_SCORE            = SEQ_EVENT_HIGH_SCORE,             // 0x12
        SEQ_ID_EVENT_MERRY_GO_ROUND        = SEQ_EVENT_MERRY_GO_ROUND,         // 0x13
        SEQ_ID_EVENT_RACE                  = SEQ_EVENT_RACE,                   // 0x14
        SEQ_ID_EVENT_CUTSCENE_STAR_SPAWN   = SEQ_EVENT_CUTSCENE_STAR_SPAWN,    // 0x15
        SEQ_ID_EVENT_BOSS                  = SEQ_EVENT_BOSS,                   // 0x16
        SEQ_ID_EVENT_CUTSCENE_COLLECT_KEY  = SEQ_EVENT_CUTSCENE_COLLECT_KEY,   // 0x17
        SEQ_ID_EVENT_ENDLESS_STAIRS        = SEQ_EVENT_ENDLESS_STAIRS,         // 0x18
        SEQ_ID_LEVEL_BOSS_KOOPA_FINAL      = SEQ_LEVEL_BOSS_KOOPA_FINAL,       // 0x19
        SEQ_ID_EVENT_CUTSCENE_CREDITS      = SEQ_EVENT_CUTSCENE_CREDITS,       // 0x1A
        SEQ_ID_EVENT_SOLVE_PUZZLE          = SEQ_EVENT_SOLVE_PUZZLE,           // 0x1B
        SEQ_ID_EVENT_TOAD_MESSAGE          = SEQ_EVENT_TOAD_MESSAGE,           // 0x1C
        SEQ_ID_EVENT_PEACH_MESSAGE         = SEQ_EVENT_PEACH_MESSAGE,          // 0x1D
        SEQ_ID_EVENT_CUTSCENE_INTRO        = SEQ_EVENT_CUTSCENE_INTRO,         // 0x1E
        SEQ_ID_EVENT_CUTSCENE_VICTORY      = SEQ_EVENT_CUTSCENE_VICTORY,       // 0x1F
        SEQ_ID_EVENT_CUTSCENE_ENDING       = SEQ_EVENT_CUTSCENE_ENDING,        // 0x20
        SEQ_ID_MENU_FILE_SELECT            = SEQ_MENU_FILE_SELECT,             // 0x21
        SEQ_ID_EVENT_CUTSCENE_LAKITU       = SEQ_EVENT_CUTSCENE_LAKITU,        // 0x22 (not in JP)
        SEQ_ID_COUNT                       = SEQ_COUNT
    };

    // void sm64_seq_player_play_sequence(uint8_t player, uint8_t seqId, uint16_t arg2);
    void play_music(SeqID p_seq_id, bool p_variant = false);
    void stop_background_music(SeqID p_seq_id);
    void stop_current_background_music();
    // void sm64_fadeout_background_music(uint16_t arg0, uint16_t fadeOut);
    SeqID get_current_background_music();
    // void sm64_play_sound(int32_t soundBits, float *pos);
    // void sm64_play_sound_global(int32_t soundBits);
    // void sm64_set_sound_volume(float vol);

    void set_internal_volume(real_t p_volume);
    // void set_reverb(int p_reverb);

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(SM64AudioStreamPlayer::SeqPlayer);
VARIANT_ENUM_CAST(SM64AudioStreamPlayer::SeqID);

#endif // LIBSM64GD_SM64AUDIOSTREAMPLAYER_H
