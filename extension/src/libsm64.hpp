#ifndef LIBSM64GD_LIBSM64_H
#define LIBSM64GD_LIBSM64_H

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/image.hpp>

#include <godot_cpp/variant/callable.hpp>

#include <libsm64.h>

class LibSM64MarioInputs;
class LibSM64SurfaceArray;

class LibSM64 : public godot::Object
{
    GDCLASS(LibSM64, godot::Object);

    inline static LibSM64 *singleton = nullptr;

public:
    LibSM64();
    ~LibSM64();

    static LibSM64 *get_singleton();
    static void register_project_settings();

    static constexpr double tick_delta_time = 1.0 / 30.0;

    // SurfaceType and TerrainType are from "libsm64/src/decomp/include/surface_terrains.h"

    enum SurfaceType
    {
        SURFACE_DEFAULT                  = 0x0000, // Environment default
        SURFACE_BURNING                  = 0x0001, // Lava / Frostbite (in SL), but is used mostly for Lava
        SURFACE_0004                     = 0x0004, // Unused, has no function and has parameters
        SURFACE_HANGABLE                 = 0x0005, // Ceiling that Mario can climb on
        SURFACE_SLOW                     = 0x0009, // Slow down Mario, unused
        SURFACE_DEATH_PLANE              = 0x000A, // Death floor
        SURFACE_CLOSE_CAMERA             = 0x000B, // Close camera
        SURFACE_WATER                    = 0x000D, // Water, has no action, used on some waterboxes below
        SURFACE_FLOWING_WATER            = 0x000E, // Water (flowing), has parameters
        SURFACE_INTANGIBLE               = 0x0012, // Intangible (Separates BBH mansion from merry-go-round, for room usage)
        SURFACE_VERY_SLIPPERY            = 0x0013, // Very slippery, mostly used for slides
        SURFACE_SLIPPERY                 = 0x0014, // Slippery
        SURFACE_NOT_SLIPPERY             = 0x0015, // Non-slippery, climbable
        SURFACE_TTM_VINES                = 0x0016, // TTM vines, has no action defined
        SURFACE_MGR_MUSIC                = 0x001A, // Plays the Merry go round music, see handle_merry_go_round_music in bbh_merry_go_round.inc.c for more details
        SURFACE_INSTANT_WARP_1B          = 0x001B, // Instant warp to another area, used to warp between areas in WDW and the endless stairs to warp back
        SURFACE_INSTANT_WARP_1C          = 0x001C, // Instant warp to another area, used to warp between areas in WDW
        SURFACE_INSTANT_WARP_1D          = 0x001D, // Instant warp to another area, used to warp between areas in DDD, SSL and TTM
        SURFACE_INSTANT_WARP_1E          = 0x001E, // Instant warp to another area, used to warp between areas in DDD, SSL and TTM
        SURFACE_SHALLOW_QUICKSAND        = 0x0021, // Shallow Quicksand (depth of 10 units)
        SURFACE_DEEP_QUICKSAND           = 0x0022, // Quicksand (lethal, slow, depth of 160 units)
        SURFACE_INSTANT_QUICKSAND        = 0x0023, // Quicksand (lethal, instant)
        SURFACE_DEEP_MOVING_QUICKSAND    = 0x0024, // Moving quicksand (flowing, depth of 160 units)
        SURFACE_SHALLOW_MOVING_QUICKSAND = 0x0025, // Moving quicksand (flowing, depth of 25 units)
        SURFACE_QUICKSAND                = 0x0026, // Moving quicksand (60 units)
        SURFACE_MOVING_QUICKSAND         = 0x0027, // Moving quicksand (flowing, depth of 60 units)
        SURFACE_WALL_MISC                = 0x0028, // Used for some walls, Cannon to adjust the camera, and some objects like Warp Pipe
        SURFACE_NOISE_DEFAULT            = 0x0029, // Default floor with noise
        SURFACE_NOISE_SLIPPERY           = 0x002A, // Slippery floor with noise
        SURFACE_HORIZONTAL_WIND          = 0x002C, // Horizontal wind, has parameters
        SURFACE_INSTANT_MOVING_QUICKSAND = 0x002D, // Quicksand (lethal, flowing)
        SURFACE_ICE                      = 0x002E, // Slippery Ice, in snow levels and THI's water floor
        SURFACE_LOOK_UP_WARP             = 0x002F, // Look up and warp (Wing cap entrance)
        SURFACE_HARD                     = 0x0030, // Hard floor (Always has fall damage)
        SURFACE_WARP                     = 0x0032, // Surface warp
        SURFACE_TIMER_START              = 0x0033, // Timer start (Peach's secret slide)
        SURFACE_TIMER_END                = 0x0034, // Timer stop (Peach's secret slide)
        SURFACE_HARD_SLIPPERY            = 0x0035, // Hard and slippery (Always has fall damage)
        SURFACE_HARD_VERY_SLIPPERY       = 0x0036, // Hard and very slippery (Always has fall damage)
        SURFACE_HARD_NOT_SLIPPERY        = 0x0037, // Hard and Non-slippery (Always has fall damage)
        SURFACE_VERTICAL_WIND            = 0x0038, // Death at bottom with vertical wind
        SURFACE_BOSS_FIGHT_CAMERA        = 0x0065, // Wide camera for BOB and WF bosses
        SURFACE_CAMERA_FREE_ROAM         = 0x0066, // Free roam camera for THI and TTC
        SURFACE_THI3_WALLKICK            = 0x0068, // Surface where there's a wall kick section in THI 3rd area, has no action defined
        SURFACE_CAMERA_8_DIR             = 0x0069, // Surface that enables far camera for platforms, used in THI
        SURFACE_CAMERA_MIDDLE            = 0x006E, // Surface camera that returns to the middle, used on the 4 pillars of SSL
        SURFACE_CAMERA_ROTATE_RIGHT      = 0x006F, // Surface camera that rotates to the right (Bowser 1 & THI)
        SURFACE_CAMERA_ROTATE_LEFT       = 0x0070, // Surface camera that rotates to the left (BOB & TTM)
        SURFACE_CAMERA_BOUNDARY          = 0x0072, // Intangible Area, only used to restrict camera movement
        SURFACE_NOISE_VERY_SLIPPERY_73   = 0x0073, // Very slippery floor with noise, unused
        SURFACE_NOISE_VERY_SLIPPERY_74   = 0x0074, // Very slippery floor with noise, unused
        SURFACE_NOISE_VERY_SLIPPERY      = 0x0075, // Very slippery floor with noise, used in CCM
        SURFACE_NO_CAM_COLLISION         = 0x0076, // Surface with no cam collision flag
        SURFACE_NO_CAM_COLLISION_77      = 0x0077, // Surface with no cam collision flag, unused
        SURFACE_NO_CAM_COL_VERY_SLIPPERY = 0x0078, // Surface with no cam collision flag, very slippery with noise (THI)
        SURFACE_NO_CAM_COL_SLIPPERY      = 0x0079, // Surface with no cam collision flag, slippery with noise (CCM, PSS and TTM slides)
        SURFACE_SWITCH                   = 0x007A, // Surface with no cam collision flag, non-slippery with noise, used by switches and Dorrie
        SURFACE_VANISH_CAP_WALLS         = 0x007B, // Vanish cap walls, pass through them with Vanish Cap
        SURFACE_PAINTING_WOBBLE_A6       = 0x00A6, // Painting wobble (BOB Left)
        SURFACE_PAINTING_WOBBLE_A7       = 0x00A7, // Painting wobble (BOB Middle)
        SURFACE_PAINTING_WOBBLE_A8       = 0x00A8, // Painting wobble (BOB Right)
        SURFACE_PAINTING_WOBBLE_A9       = 0x00A9, // Painting wobble (CCM Left)
        SURFACE_PAINTING_WOBBLE_AA       = 0x00AA, // Painting wobble (CCM Middle)
        SURFACE_PAINTING_WOBBLE_AB       = 0x00AB, // Painting wobble (CCM Right)
        SURFACE_PAINTING_WOBBLE_AC       = 0x00AC, // Painting wobble (WF Left)
        SURFACE_PAINTING_WOBBLE_AD       = 0x00AD, // Painting wobble (WF Middle)
        SURFACE_PAINTING_WOBBLE_AE       = 0x00AE, // Painting wobble (WF Right)
        SURFACE_PAINTING_WOBBLE_AF       = 0x00AF, // Painting wobble (JRB Left)
        SURFACE_PAINTING_WOBBLE_B0       = 0x00B0, // Painting wobble (JRB Middle)
        SURFACE_PAINTING_WOBBLE_B1       = 0x00B1, // Painting wobble (JRB Right)
        SURFACE_PAINTING_WOBBLE_B2       = 0x00B2, // Painting wobble (LLL Left)
        SURFACE_PAINTING_WOBBLE_B3       = 0x00B3, // Painting wobble (LLL Middle)
        SURFACE_PAINTING_WOBBLE_B4       = 0x00B4, // Painting wobble (LLL Right)
        SURFACE_PAINTING_WOBBLE_B5       = 0x00B5, // Painting wobble (SSL Left)
        SURFACE_PAINTING_WOBBLE_B6       = 0x00B6, // Painting wobble (SSL Middle)
        SURFACE_PAINTING_WOBBLE_B7       = 0x00B7, // Painting wobble (SSL Right)
        SURFACE_PAINTING_WOBBLE_B8       = 0x00B8, // Painting wobble (Unused - Left)
        SURFACE_PAINTING_WOBBLE_B9       = 0x00B9, // Painting wobble (Unused - Middle)
        SURFACE_PAINTING_WOBBLE_BA       = 0x00BA, // Painting wobble (Unused - Right)
        SURFACE_PAINTING_WOBBLE_BB       = 0x00BB, // Painting wobble (DDD - Left), makes the painting wobble if touched
        SURFACE_PAINTING_WOBBLE_BC       = 0x00BC, // Painting wobble (Unused, DDD - Middle)
        SURFACE_PAINTING_WOBBLE_BD       = 0x00BD, // Painting wobble (Unused, DDD - Right)
        SURFACE_PAINTING_WOBBLE_BE       = 0x00BE, // Painting wobble (WDW Left)
        SURFACE_PAINTING_WOBBLE_BF       = 0x00BF, // Painting wobble (WDW Middle)
        SURFACE_PAINTING_WOBBLE_C0       = 0x00C0, // Painting wobble (WDW Right)
        SURFACE_PAINTING_WOBBLE_C1       = 0x00C1, // Painting wobble (THI Tiny - Left)
        SURFACE_PAINTING_WOBBLE_C2       = 0x00C2, // Painting wobble (THI Tiny - Middle)
        SURFACE_PAINTING_WOBBLE_C3       = 0x00C3, // Painting wobble (THI Tiny - Right)
        SURFACE_PAINTING_WOBBLE_C4       = 0x00C4, // Painting wobble (TTM Left)
        SURFACE_PAINTING_WOBBLE_C5       = 0x00C5, // Painting wobble (TTM Middle)
        SURFACE_PAINTING_WOBBLE_C6       = 0x00C6, // Painting wobble (TTM Right)
        SURFACE_PAINTING_WOBBLE_C7       = 0x00C7, // Painting wobble (Unused, TTC - Left)
        SURFACE_PAINTING_WOBBLE_C8       = 0x00C8, // Painting wobble (Unused, TTC - Middle)
        SURFACE_PAINTING_WOBBLE_C9       = 0x00C9, // Painting wobble (Unused, TTC - Right)
        SURFACE_PAINTING_WOBBLE_CA       = 0x00CA, // Painting wobble (Unused, SL - Left)
        SURFACE_PAINTING_WOBBLE_CB       = 0x00CB, // Painting wobble (Unused, SL - Middle)
        SURFACE_PAINTING_WOBBLE_CC       = 0x00CC, // Painting wobble (Unused, SL - Right)
        SURFACE_PAINTING_WOBBLE_CD       = 0x00CD, // Painting wobble (THI Huge - Left)
        SURFACE_PAINTING_WOBBLE_CE       = 0x00CE, // Painting wobble (THI Huge - Middle)
        SURFACE_PAINTING_WOBBLE_CF       = 0x00CF, // Painting wobble (THI Huge - Right)
        SURFACE_PAINTING_WOBBLE_D0       = 0x00D0, // Painting wobble (HMC & COTMC - Left), makes the painting wobble if touched
        SURFACE_PAINTING_WOBBLE_D1       = 0x00D1, // Painting wobble (Unused, HMC & COTMC - Middle)
        SURFACE_PAINTING_WOBBLE_D2       = 0x00D2, // Painting wobble (Unused, HMC & COTMC - Right)
        SURFACE_PAINTING_WARP_D3         = 0x00D3, // Painting warp (BOB Left)
        SURFACE_PAINTING_WARP_D4         = 0x00D4, // Painting warp (BOB Middle)
        SURFACE_PAINTING_WARP_D5         = 0x00D5, // Painting warp (BOB Right)
        SURFACE_PAINTING_WARP_D6         = 0x00D6, // Painting warp (CCM Left)
        SURFACE_PAINTING_WARP_D7         = 0x00D7, // Painting warp (CCM Middle)
        SURFACE_PAINTING_WARP_D8         = 0x00D8, // Painting warp (CCM Right)
        SURFACE_PAINTING_WARP_D9         = 0x00D9, // Painting warp (WF Left)
        SURFACE_PAINTING_WARP_DA         = 0x00DA, // Painting warp (WF Middle)
        SURFACE_PAINTING_WARP_DB         = 0x00DB, // Painting warp (WF Right)
        SURFACE_PAINTING_WARP_DC         = 0x00DC, // Painting warp (JRB Left)
        SURFACE_PAINTING_WARP_DD         = 0x00DD, // Painting warp (JRB Middle)
        SURFACE_PAINTING_WARP_DE         = 0x00DE, // Painting warp (JRB Right)
        SURFACE_PAINTING_WARP_DF         = 0x00DF, // Painting warp (LLL Left)
        SURFACE_PAINTING_WARP_E0         = 0x00E0, // Painting warp (LLL Middle)
        SURFACE_PAINTING_WARP_E1         = 0x00E1, // Painting warp (LLL Right)
        SURFACE_PAINTING_WARP_E2         = 0x00E2, // Painting warp (SSL Left)
        SURFACE_PAINTING_WARP_E3         = 0x00E3, // Painting warp (SSL Medium)
        SURFACE_PAINTING_WARP_E4         = 0x00E4, // Painting warp (SSL Right)
        SURFACE_PAINTING_WARP_E5         = 0x00E5, // Painting warp (Unused - Left)
        SURFACE_PAINTING_WARP_E6         = 0x00E6, // Painting warp (Unused - Medium)
        SURFACE_PAINTING_WARP_E7         = 0x00E7, // Painting warp (Unused - Right)
        SURFACE_PAINTING_WARP_E8         = 0x00E8, // Painting warp (DDD - Left)
        SURFACE_PAINTING_WARP_E9         = 0x00E9, // Painting warp (DDD - Middle)
        SURFACE_PAINTING_WARP_EA         = 0x00EA, // Painting warp (DDD - Right)
        SURFACE_PAINTING_WARP_EB         = 0x00EB, // Painting warp (WDW Left)
        SURFACE_PAINTING_WARP_EC         = 0x00EC, // Painting warp (WDW Middle)
        SURFACE_PAINTING_WARP_ED         = 0x00ED, // Painting warp (WDW Right)
        SURFACE_PAINTING_WARP_EE         = 0x00EE, // Painting warp (THI Tiny - Left)
        SURFACE_PAINTING_WARP_EF         = 0x00EF, // Painting warp (THI Tiny - Middle)
        SURFACE_PAINTING_WARP_F0         = 0x00F0, // Painting warp (THI Tiny - Right)
        SURFACE_PAINTING_WARP_F1         = 0x00F1, // Painting warp (TTM Left)
        SURFACE_PAINTING_WARP_F2         = 0x00F2, // Painting warp (TTM Middle)
        SURFACE_PAINTING_WARP_F3         = 0x00F3, // Painting warp (TTM Right)
        SURFACE_TTC_PAINTING_1           = 0x00F4, // Painting warp (TTC Left)
        SURFACE_TTC_PAINTING_2           = 0x00F5, // Painting warp (TTC Medium)
        SURFACE_TTC_PAINTING_3           = 0x00F6, // Painting warp (TTC Right)
        SURFACE_PAINTING_WARP_F7         = 0x00F7, // Painting warp (SL Left)
        SURFACE_PAINTING_WARP_F8         = 0x00F8, // Painting warp (SL Middle)
        SURFACE_PAINTING_WARP_F9         = 0x00F9, // Painting warp (SL Right)
        SURFACE_PAINTING_WARP_FA         = 0x00FA, // Painting warp (THI Tiny - Left)
        SURFACE_PAINTING_WARP_FB         = 0x00FB, // Painting warp (THI Tiny - Middle)
        SURFACE_PAINTING_WARP_FC         = 0x00FC, // Painting warp (THI Tiny - Right)
        SURFACE_WOBBLING_WARP            = 0x00FD, // Pool warp (HMC & DDD)
        SURFACE_TRAPDOOR                 = 0x00FF, // Bowser Left trapdoor, has no action defined
    };

    enum TerrainType
    {
        TERRAIN_GRASS  = 0x0000,
        TERRAIN_STONE  = 0x0001,
        TERRAIN_SNOW   = 0x0002,
        TERRAIN_SAND   = 0x0003,
        TERRAIN_SPOOKY = 0x0004,
        TERRAIN_WATER  = 0x0005,
        TERRAIN_SLIDE  = 0x0006,
        TERRAIN_MASK   = 0x0007,
    };

    // ParticleFlags, MarioFlags, ActionFlags are from "libsm64/src/decomp/include/sm64.h"

    enum ParticleFlags
    {
        PARTICLE_DUST                 /* 0x00000001 */ = (1 <<  0),
        PARTICLE_VERTICAL_STAR        /* 0x00000002 */ = (1 <<  1),
        PARTICLE_2                    /* 0x00000004 */ = (1 <<  2),
        PARTICLE_SPARKLES             /* 0x00000008 */ = (1 <<  3),
        PARTICLE_HORIZONTAL_STAR      /* 0x00000010 */ = (1 <<  4),
        PARTICLE_BUBBLE               /* 0x00000020 */ = (1 <<  5),
        PARTICLE_WATER_SPLASH         /* 0x00000040 */ = (1 <<  6),
        PARTICLE_IDLE_WATER_WAVE      /* 0x00000080 */ = (1 <<  7),
        PARTICLE_SHALLOW_WATER_WAVE   /* 0x00000100 */ = (1 <<  8),
        PARTICLE_PLUNGE_BUBBLE        /* 0x00000200 */ = (1 <<  9),
        PARTICLE_WAVE_TRAIL           /* 0x00000400 */ = (1 << 10),
        PARTICLE_FIRE                 /* 0x00000800 */ = (1 << 11),
        PARTICLE_SHALLOW_WATER_SPLASH /* 0x00001000 */ = (1 << 12),
        PARTICLE_LEAF                 /* 0x00002000 */ = (1 << 13),
        PARTICLE_SNOW                 /* 0x00004000 */ = (1 << 14),
        PARTICLE_DIRT                 /* 0x00008000 */ = (1 << 15),
        PARTICLE_MIST_CIRCLE          /* 0x00010000 */ = (1 << 16),
        PARTICLE_BREATH               /* 0x00020000 */ = (1 << 17),
        PARTICLE_TRIANGLE             /* 0x00040000 */ = (1 << 18),
        PARTICLE_19                   /* 0x00080000 */ = (1 << 19),
    };

    enum MarioFlags
    {
        MARIO_NORMAL_CAP          = 0x00000001,
        MARIO_VANISH_CAP          = 0x00000002,
        MARIO_METAL_CAP           = 0x00000004,
        MARIO_WING_CAP            = 0x00000008,
        MARIO_CAP_ON_HEAD         = 0x00000010,
        MARIO_CAP_IN_HAND         = 0x00000020,
        MARIO_METAL_SHOCK         = 0x00000040,
        MARIO_TELEPORTING         = 0x00000080,
        MARIO_UNKNOWN_08          = 0x00000100,
        MARIO_UNKNOWN_13          = 0x00002000,
        MARIO_ACTION_SOUND_PLAYED = 0x00010000,
        MARIO_MARIO_SOUND_PLAYED  = 0x00020000,
        MARIO_UNKNOWN_18          = 0x00040000,
        MARIO_PUNCHING            = 0x00100000,
        MARIO_KICKING             = 0x00200000,
        MARIO_TRIPPING            = 0x00400000,
        MARIO_UNKNOWN_25          = 0x02000000,
        MARIO_UNKNOWN_30          = 0x40000000,
        MARIO_UNKNOWN_31          = 0x80000000,

        MARIO_SPECIAL_CAPS        = (MARIO_VANISH_CAP | MARIO_METAL_CAP | MARIO_WING_CAP),
        MARIO_CAPS                = (MARIO_NORMAL_CAP | MARIO_SPECIAL_CAPS),
    };

    enum ActionFlags
    {
        ACT_ID_MASK = 0x000001FF,

        ACT_GROUP_MASK                        = 0x000001C0,
        ACT_GROUP_STATIONARY /* 0x00000000 */ = (0 << 6),
        ACT_GROUP_MOVING     /* 0x00000040 */ = (1 << 6),
        ACT_GROUP_AIRBORNE   /* 0x00000080 */ = (2 << 6),
        ACT_GROUP_SUBMERGED  /* 0x000000C0 */ = (3 << 6),
        ACT_GROUP_CUTSCENE   /* 0x00000100 */ = (4 << 6),
        ACT_GROUP_AUTOMATIC  /* 0x00000140 */ = (5 << 6),
        ACT_GROUP_OBJECT     /* 0x00000180 */ = (6 << 6),

        ACT_FLAG_STATIONARY                  /* 0x00000200 */ = (1 <<  9),
        ACT_FLAG_MOVING                      /* 0x00000400 */ = (1 << 10),
        ACT_FLAG_AIR                         /* 0x00000800 */ = (1 << 11),
        ACT_FLAG_INTANGIBLE                  /* 0x00001000 */ = (1 << 12),
        ACT_FLAG_SWIMMING                    /* 0x00002000 */ = (1 << 13),
        ACT_FLAG_METAL_WATER                 /* 0x00004000 */ = (1 << 14),
        ACT_FLAG_SHORT_HITBOX                /* 0x00008000 */ = (1 << 15),
        ACT_FLAG_RIDING_SHELL                /* 0x00010000 */ = (1 << 16),
        ACT_FLAG_INVULNERABLE                /* 0x00020000 */ = (1 << 17),
        ACT_FLAG_BUTT_OR_STOMACH_SLIDE       /* 0x00040000 */ = (1 << 18),
        ACT_FLAG_DIVING                      /* 0x00080000 */ = (1 << 19),
        ACT_FLAG_ON_POLE                     /* 0x00100000 */ = (1 << 20),
        ACT_FLAG_HANGING                     /* 0x00200000 */ = (1 << 21),
        ACT_FLAG_IDLE                        /* 0x00400000 */ = (1 << 22),
        ACT_FLAG_ATTACKING                   /* 0x00800000 */ = (1 << 23),
        ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION  /* 0x01000000 */ = (1 << 24),
        ACT_FLAG_CONTROL_JUMP_HEIGHT         /* 0x02000000 */ = (1 << 25),
        ACT_FLAG_ALLOW_FIRST_PERSON          /* 0x04000000 */ = (1 << 26),
        ACT_FLAG_PAUSE_EXIT                  /* 0x08000000 */ = (1 << 27),
        ACT_FLAG_SWIMMING_OR_FLYING          /* 0x10000000 */ = (1 << 28),
        ACT_FLAG_WATER_OR_TEXT               /* 0x20000000 */ = (1 << 29),
        ACT_FLAG_THROWING                    /* 0x80000000 */ = (1 << 31),

        ACT_UNINITIALIZED              = 0x00000000, // (0x000)

    // group 0x000: stationary actions
        ACT_IDLE                       = 0x0C400201, // (0x001 | ACT_FLAG_STATIONARY | ACT_FLAG_IDLE | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_START_SLEEPING             = 0x0C400202, // (0x002 | ACT_FLAG_STATIONARY | ACT_FLAG_IDLE | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_SLEEPING                   = 0x0C000203, // (0x003 | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_WAKING_UP                  = 0x0C000204, // (0x004 | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_PANTING                    = 0x0C400205, // (0x005 | ACT_FLAG_STATIONARY | ACT_FLAG_IDLE | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_HOLD_PANTING_UNUSED        = 0x08000206, // (0x006 | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_HOLD_IDLE                  = 0x08000207, // (0x007 | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_HOLD_HEAVY_IDLE            = 0x08000208, // (0x008 | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_STANDING_AGAINST_WALL      = 0x0C400209, // (0x009 | ACT_FLAG_STATIONARY | ACT_FLAG_IDLE | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_COUGHING                   = 0x0C40020A, // (0x00A | ACT_FLAG_STATIONARY | ACT_FLAG_IDLE | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_SHIVERING                  = 0x0C40020B, // (0x00B | ACT_FLAG_STATIONARY | ACT_FLAG_IDLE | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_IN_QUICKSAND               = 0x0002020D, // (0x00D | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_UNKNOWN_0002020E           = 0x0002020E, // (0x00E | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_CROUCHING                  = 0x0C008220, // (0x020 | ACT_FLAG_STATIONARY | ACT_FLAG_SHORT_HITBOX | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_START_CROUCHING            = 0x0C008221, // (0x021 | ACT_FLAG_STATIONARY | ACT_FLAG_SHORT_HITBOX | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_STOP_CROUCHING             = 0x0C008222, // (0x022 | ACT_FLAG_STATIONARY | ACT_FLAG_SHORT_HITBOX | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_START_CRAWLING             = 0x0C008223, // (0x023 | ACT_FLAG_STATIONARY | ACT_FLAG_SHORT_HITBOX | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_STOP_CRAWLING              = 0x0C008224, // (0x024 | ACT_FLAG_STATIONARY | ACT_FLAG_SHORT_HITBOX | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_SLIDE_KICK_SLIDE_STOP      = 0x08000225, // (0x025 | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_SHOCKWAVE_BOUNCE           = 0x00020226, // (0x026 | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_FIRST_PERSON               = 0x0C000227, // (0x027 | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_BACKFLIP_LAND_STOP         = 0x0800022F, // (0x02F | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_JUMP_LAND_STOP             = 0x0C000230, // (0x030 | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_DOUBLE_JUMP_LAND_STOP      = 0x0C000231, // (0x031 | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_FREEFALL_LAND_STOP         = 0x0C000232, // (0x032 | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_SIDE_FLIP_LAND_STOP        = 0x0C000233, // (0x033 | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_HOLD_JUMP_LAND_STOP        = 0x08000234, // (0x034 | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_HOLD_FREEFALL_LAND_STOP    = 0x08000235, // (0x035 | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_AIR_THROW_LAND             = 0x80000A36, // (0x036 | ACT_FLAG_STATIONARY | ACT_FLAG_AIR | ACT_FLAG_THROWING)
        ACT_TWIRL_LAND                 = 0x18800238, // (0x038 | ACT_FLAG_STATIONARY | ACT_FLAG_ATTACKING | ACT_FLAG_PAUSE_EXIT | ACT_FLAG_SWIMMING_OR_FLYING)
        ACT_LAVA_BOOST_LAND            = 0x08000239, // (0x039 | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_TRIPLE_JUMP_LAND_STOP      = 0x0800023A, // (0x03A | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_LONG_JUMP_LAND_STOP        = 0x0800023B, // (0x03B | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_GROUND_POUND_LAND          = 0x0080023C, // (0x03C | ACT_FLAG_STATIONARY | ACT_FLAG_ATTACKING)
        ACT_BRAKING_STOP               = 0x0C00023D, // (0x03D | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_BUTT_SLIDE_STOP            = 0x0C00023E, // (0x03E | ACT_FLAG_STATIONARY | ACT_FLAG_ALLOW_FIRST_PERSON | ACT_FLAG_PAUSE_EXIT)
        ACT_HOLD_BUTT_SLIDE_STOP       = 0x0800043F, // (0x03F | ACT_FLAG_MOVING | ACT_FLAG_PAUSE_EXIT)

    // group 0x040: moving (ground) actions
        ACT_WALKING                    = 0x04000440, // (0x040 | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_HOLD_WALKING               = 0x00000442, // (0x042 | ACT_FLAG_MOVING)
        ACT_TURNING_AROUND             = 0x00000443, // (0x043 | ACT_FLAG_MOVING)
        ACT_FINISH_TURNING_AROUND      = 0x00000444, // (0x044 | ACT_FLAG_MOVING)
        ACT_BRAKING                    = 0x04000445, // (0x045 | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_RIDING_SHELL_GROUND        = 0x20810446, // (0x046 | ACT_FLAG_MOVING | ACT_FLAG_RIDING_SHELL | ACT_FLAG_ATTACKING | ACT_FLAG_WATER_OR_TEXT)
        ACT_HOLD_HEAVY_WALKING         = 0x00000447, // (0x047 | ACT_FLAG_MOVING)
        ACT_CRAWLING                   = 0x04008448, // (0x048 | ACT_FLAG_MOVING | ACT_FLAG_SHORT_HITBOX | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_BURNING_GROUND             = 0x00020449, // (0x049 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_DECELERATING               = 0x0400044A, // (0x04A | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_HOLD_DECELERATING          = 0x0000044B, // (0x04B | ACT_FLAG_MOVING)
        ACT_BEGIN_SLIDING              = 0x00000050, // (0x050)
        ACT_HOLD_BEGIN_SLIDING         = 0x00000051, // (0x051)
        ACT_BUTT_SLIDE                 = 0x00840452, // (0x052 | ACT_FLAG_MOVING | ACT_FLAG_BUTT_OR_STOMACH_SLIDE | ACT_FLAG_ATTACKING)
        ACT_STOMACH_SLIDE              = 0x008C0453, // (0x053 | ACT_FLAG_MOVING | ACT_FLAG_BUTT_OR_STOMACH_SLIDE | ACT_FLAG_DIVING | ACT_FLAG_ATTACKING)
        ACT_HOLD_BUTT_SLIDE            = 0x00840454, // (0x054 | ACT_FLAG_MOVING | ACT_FLAG_BUTT_OR_STOMACH_SLIDE | ACT_FLAG_ATTACKING)
        ACT_HOLD_STOMACH_SLIDE         = 0x008C0455, // (0x055 | ACT_FLAG_MOVING | ACT_FLAG_BUTT_OR_STOMACH_SLIDE | ACT_FLAG_DIVING | ACT_FLAG_ATTACKING)
        ACT_DIVE_SLIDE                 = 0x00880456, // (0x056 | ACT_FLAG_MOVING | ACT_FLAG_DIVING | ACT_FLAG_ATTACKING)
        ACT_MOVE_PUNCHING              = 0x00800457, // (0x057 | ACT_FLAG_MOVING | ACT_FLAG_ATTACKING)
        ACT_CROUCH_SLIDE               = 0x04808459, // (0x059 | ACT_FLAG_MOVING | ACT_FLAG_SHORT_HITBOX | ACT_FLAG_ATTACKING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_SLIDE_KICK_SLIDE           = 0x0080045A, // (0x05A | ACT_FLAG_MOVING | ACT_FLAG_ATTACKING)
        ACT_HARD_BACKWARD_GROUND_KB    = 0x00020460, // (0x060 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_HARD_FORWARD_GROUND_KB     = 0x00020461, // (0x061 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_BACKWARD_GROUND_KB         = 0x00020462, // (0x062 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_FORWARD_GROUND_KB          = 0x00020463, // (0x063 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_SOFT_BACKWARD_GROUND_KB    = 0x00020464, // (0x064 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_SOFT_FORWARD_GROUND_KB     = 0x00020465, // (0x065 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_GROUND_BONK                = 0x00020466, // (0x066 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_DEATH_EXIT_LAND            = 0x00020467, // (0x067 | ACT_FLAG_MOVING | ACT_FLAG_INVULNERABLE)
        ACT_JUMP_LAND                  = 0x04000470, // (0x070 | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_FREEFALL_LAND              = 0x04000471, // (0x071 | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_DOUBLE_JUMP_LAND           = 0x04000472, // (0x072 | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_SIDE_FLIP_LAND             = 0x04000473, // (0x073 | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_HOLD_JUMP_LAND             = 0x00000474, // (0x074 | ACT_FLAG_MOVING)
        ACT_HOLD_FREEFALL_LAND         = 0x00000475, // (0x075 | ACT_FLAG_MOVING)
        ACT_QUICKSAND_JUMP_LAND        = 0x00000476, // (0x076 | ACT_FLAG_MOVING)
        ACT_HOLD_QUICKSAND_JUMP_LAND   = 0x00000477, // (0x077 | ACT_FLAG_MOVING)
        ACT_TRIPLE_JUMP_LAND           = 0x04000478, // (0x078 | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_LONG_JUMP_LAND             = 0x00000479, // (0x079 | ACT_FLAG_MOVING)
        ACT_BACKFLIP_LAND              = 0x0400047A, // (0x07A | ACT_FLAG_MOVING | ACT_FLAG_ALLOW_FIRST_PERSON)

    // group 0x080: airborne actions
        ACT_JUMP                       = 0x03000880, // (0x080 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_DOUBLE_JUMP                = 0x03000881, // (0x081 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_TRIPLE_JUMP                = 0x01000882, // (0x082 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_BACKFLIP                   = 0x01000883, // (0x083 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_STEEP_JUMP                 = 0x03000885, // (0x085 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_WALL_KICK_AIR              = 0x03000886, // (0x086 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_SIDE_FLIP                  = 0x01000887, // (0x087 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_LONG_JUMP                  = 0x03000888, // (0x088 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_WATER_JUMP                 = 0x01000889, // (0x089 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_DIVE                       = 0x0188088A, // (0x08A | ACT_FLAG_AIR | ACT_FLAG_DIVING | ACT_FLAG_ATTACKING | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_FREEFALL                   = 0x0100088C, // (0x08C | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_TOP_OF_POLE_JUMP           = 0x0300088D, // (0x08D | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_BUTT_SLIDE_AIR             = 0x0300088E, // (0x08E | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_FLYING_TRIPLE_JUMP         = 0x03000894, // (0x094 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_SHOT_FROM_CANNON           = 0x00880898, // (0x098 | ACT_FLAG_AIR | ACT_FLAG_DIVING | ACT_FLAG_ATTACKING)
        ACT_FLYING                     = 0x10880899, // (0x099 | ACT_FLAG_AIR | ACT_FLAG_DIVING | ACT_FLAG_ATTACKING | ACT_FLAG_SWIMMING_OR_FLYING)
        ACT_RIDING_SHELL_JUMP          = 0x0281089A, // (0x09A | ACT_FLAG_AIR | ACT_FLAG_RIDING_SHELL | ACT_FLAG_ATTACKING | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_RIDING_SHELL_FALL          = 0x0081089B, // (0x09B | ACT_FLAG_AIR | ACT_FLAG_RIDING_SHELL | ACT_FLAG_ATTACKING)
        ACT_VERTICAL_WIND              = 0x1008089C, // (0x09C | ACT_FLAG_AIR | ACT_FLAG_DIVING | ACT_FLAG_SWIMMING_OR_FLYING)
        ACT_HOLD_JUMP                  = 0x030008A0, // (0x0A0 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_HOLD_FREEFALL              = 0x010008A1, // (0x0A1 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_HOLD_BUTT_SLIDE_AIR        = 0x010008A2, // (0x0A2 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_HOLD_WATER_JUMP            = 0x010008A3, // (0x0A3 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_TWIRLING                   = 0x108008A4, // (0x0A4 | ACT_FLAG_AIR | ACT_FLAG_ATTACKING | ACT_FLAG_SWIMMING_OR_FLYING)
        ACT_FORWARD_ROLLOUT            = 0x010008A6, // (0x0A6 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_AIR_HIT_WALL               = 0x000008A7, // (0x0A7 | ACT_FLAG_AIR)
        ACT_RIDING_HOOT                = 0x000004A8, // (0x0A8 | ACT_FLAG_MOVING)
        ACT_GROUND_POUND               = 0x008008A9, // (0x0A9 | ACT_FLAG_AIR | ACT_FLAG_ATTACKING)
        ACT_SLIDE_KICK                 = 0x018008AA, // (0x0AA | ACT_FLAG_AIR | ACT_FLAG_ATTACKING | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_AIR_THROW                  = 0x830008AB, // (0x0AB | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT | ACT_FLAG_THROWING)
        ACT_JUMP_KICK                  = 0x018008AC, // (0x0AC | ACT_FLAG_AIR | ACT_FLAG_ATTACKING | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_BACKWARD_ROLLOUT           = 0x010008AD, // (0x0AD | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_CRAZY_BOX_BOUNCE           = 0x000008AE, // (0x0AE | ACT_FLAG_AIR)
        ACT_SPECIAL_TRIPLE_JUMP        = 0x030008AF, // (0x0AF | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION | ACT_FLAG_CONTROL_JUMP_HEIGHT)
        ACT_BACKWARD_AIR_KB            = 0x010208B0, // (0x0B0 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_FORWARD_AIR_KB             = 0x010208B1, // (0x0B1 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_HARD_FORWARD_AIR_KB        = 0x010208B2, // (0x0B2 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_HARD_BACKWARD_AIR_KB       = 0x010208B3, // (0x0B3 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_BURNING_JUMP               = 0x010208B4, // (0x0B4 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_BURNING_FALL               = 0x010208B5, // (0x0B5 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_SOFT_BONK                  = 0x010208B6, // (0x0B6 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_LAVA_BOOST                 = 0x010208B7, // (0x0B7 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_GETTING_BLOWN              = 0x010208B8, // (0x0B8 | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_THROWN_FORWARD             = 0x010208BD, // (0x0BD | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
        ACT_THROWN_BACKWARD            = 0x010208BE, // (0x0BE | ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ALLOW_VERTICAL_WIND_

    // group 0x0C0: submerged actions
        ACT_WATER_IDLE                 = 0x380022C0, // (0x0C0 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_PAUSE_EXIT | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_HOLD_WATER_IDLE            = 0x380022C1, // (0x0C1 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_PAUSE_EXIT | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_WATER_ACTION_END           = 0x300022C2, // (0x0C2 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_HOLD_WATER_ACTION_END      = 0x300022C3, // (0x0C3 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_DROWNING                   = 0x300032C4, // (0x0C4 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_BACKWARD_WATER_KB          = 0x300222C5, // (0x0C5 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_INVULNERABLE | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_FORWARD_WATER_KB           = 0x300222C6, // (0x0C6 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_INVULNERABLE | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_WATER_DEATH                = 0x300032C7, // (0x0C7 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_WATER_SHOCKED              = 0x300222C8, // (0x0C8 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_INVULNERABLE | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_BREASTSTROKE               = 0x300024D0, // (0x0D0 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_SWIMMING_END               = 0x300024D1, // (0x0D1 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_FLUTTER_KICK               = 0x300024D2, // (0x0D2 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_HOLD_BREASTSTROKE          = 0x300024D3, // (0x0D3 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_HOLD_SWIMMING_END          = 0x300024D4, // (0x0D4 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_HOLD_FLUTTER_KICK          = 0x300024D5, // (0x0D5 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_WATER_SHELL_SWIMMING       = 0x300024D6, // (0x0D6 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_WATER_THROW                = 0x300024E0, // (0x0E0 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_WATER_PUNCH                = 0x300024E1, // (0x0E1 | ACT_FLAG_MOVING | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_WATER_PLUNGE               = 0x300022E2, // (0x0E2 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_CAUGHT_IN_WHIRLPOOL        = 0x300222E3, // (0x0E3 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_INVULNERABLE | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
        ACT_METAL_WATER_STANDING       = 0x080042F0, // (0x0F0 | ACT_FLAG_STATIONARY | ACT_FLAG_METAL_WATER | ACT_FLAG_PAUSE_EXIT)
        ACT_HOLD_METAL_WATER_STANDING  = 0x080042F1, // (0x0F1 | ACT_FLAG_STATIONARY | ACT_FLAG_METAL_WATER | ACT_FLAG_PAUSE_EXIT)
        ACT_METAL_WATER_WALKING        = 0x000044F2, // (0x0F2 | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)
        ACT_HOLD_METAL_WATER_WALKING   = 0x000044F3, // (0x0F3 | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)
        ACT_METAL_WATER_FALLING        = 0x000042F4, // (0x0F4 | ACT_FLAG_STATIONARY | ACT_FLAG_METAL_WATER)
        ACT_HOLD_METAL_WATER_FALLING   = 0x000042F5, // (0x0F5 | ACT_FLAG_STATIONARY | ACT_FLAG_METAL_WATER)
        ACT_METAL_WATER_FALL_LAND      = 0x000042F6, // (0x0F6 | ACT_FLAG_STATIONARY | ACT_FLAG_METAL_WATER)
        ACT_HOLD_METAL_WATER_FALL_LAND = 0x000042F7, // (0x0F7 | ACT_FLAG_STATIONARY | ACT_FLAG_METAL_WATER)
        ACT_METAL_WATER_JUMP           = 0x000044F8, // (0x0F8 | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)
        ACT_HOLD_METAL_WATER_JUMP      = 0x000044F9, // (0x0F9 | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)
        ACT_METAL_WATER_JUMP_LAND      = 0x000044FA, // (0x0FA | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)
        ACT_HOLD_METAL_WATER_JUMP_LAND = 0x000044FB, // (0x0FB | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)

    // group 0x100: cutscene actions
        ACT_DISAPPEARED                = 0x00001300, // (0x100 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_INTRO_CUTSCENE             = 0x04001301, // (0x101 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_ALLOW_FIRST_PERSON)
        ACT_STAR_DANCE_EXIT            = 0x00001302, // (0x102 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_STAR_DANCE_WATER           = 0x00001303, // (0x103 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_FALL_AFTER_STAR_GRAB       = 0x00001904, // (0x104 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_READING_AUTOMATIC_DIALOG   = 0x20001305, // (0x105 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_WATER_OR_TEXT)
        ACT_READING_NPC_DIALOG         = 0x20001306, // (0x106 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_WATER_OR_TEXT)
        ACT_STAR_DANCE_NO_EXIT         = 0x00001307, // (0x107 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_READING_SIGN               = 0x00001308, // (0x108 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_JUMBO_STAR_CUTSCENE        = 0x00001909, // (0x109 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_WAITING_FOR_DIALOG         = 0x0000130A, // (0x10A | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_DEBUG_FREE_MOVE            = 0x0000130F, // (0x10F | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_STANDING_DEATH             = 0x00021311, // (0x111 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)
        ACT_QUICKSAND_DEATH            = 0x00021312, // (0x112 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)
        ACT_ELECTROCUTION              = 0x00021313, // (0x113 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)
        ACT_SUFFOCATION                = 0x00021314, // (0x114 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)
        ACT_DEATH_ON_STOMACH           = 0x00021315, // (0x115 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)
        ACT_DEATH_ON_BACK              = 0x00021316, // (0x116 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)
        ACT_EATEN_BY_BUBBA             = 0x00021317, // (0x117 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)
        ACT_END_PEACH_CUTSCENE         = 0x00001918, // (0x118 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_CREDITS_CUTSCENE           = 0x00001319, // (0x119 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_END_WAVING_CUTSCENE        = 0x0000131A, // (0x11A | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_PULLING_DOOR               = 0x00001320, // (0x120 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_PUSHING_DOOR               = 0x00001321, // (0x121 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_WARP_DOOR_SPAWN            = 0x00001322, // (0x122 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_EMERGE_FROM_PIPE           = 0x00001923, // (0x123 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_SPAWN_SPIN_AIRBORNE        = 0x00001924, // (0x124 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_SPAWN_SPIN_LANDING         = 0x00001325, // (0x125 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_EXIT_AIRBORNE              = 0x00001926, // (0x126 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_EXIT_LAND_SAVE_DIALOG      = 0x00001327, // (0x127 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_DEATH_EXIT                 = 0x00001928, // (0x128 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_UNUSED_DEATH_EXIT          = 0x00001929, // (0x129 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_FALLING_DEATH_EXIT         = 0x0000192A, // (0x12A | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_SPECIAL_EXIT_AIRBORNE      = 0x0000192B, // (0x12B | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_SPECIAL_DEATH_EXIT         = 0x0000192C, // (0x12C | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_FALLING_EXIT_AIRBORNE      = 0x0000192D, // (0x12D | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_UNLOCKING_KEY_DOOR         = 0x0000132E, // (0x12E | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_UNLOCKING_STAR_DOOR        = 0x0000132F, // (0x12F | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_ENTERING_STAR_DOOR         = 0x00001331, // (0x131 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_SPAWN_NO_SPIN_AIRBORNE     = 0x00001932, // (0x132 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_SPAWN_NO_SPIN_LANDING      = 0x00001333, // (0x133 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_BBH_ENTER_JUMP             = 0x00001934, // (0x134 | ACT_FLAG_AIR | ACT_FLAG_INTANGIBLE)
        ACT_BBH_ENTER_SPIN             = 0x00001535, // (0x135 | ACT_FLAG_MOVING | ACT_FLAG_INTANGIBLE)
        ACT_TELEPORT_FADE_OUT          = 0x00001336, // (0x136 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_TELEPORT_FADE_IN           = 0x00001337, // (0x137 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_SHOCKED                    = 0x00020338, // (0x138 | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_SQUISHED                   = 0x00020339, // (0x139 | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_HEAD_STUCK_IN_GROUND       = 0x0002033A, // (0x13A | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_BUTT_STUCK_IN_GROUND       = 0x0002033B, // (0x13B | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_FEET_STUCK_IN_GROUND       = 0x0002033C, // (0x13C | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_PUTTING_ON_CAP             = 0x0000133D, // (0x13D | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)

    // group 0x140: "automatic" actions
        ACT_HOLDING_POLE               = 0x08100340, // (0x140 | ACT_FLAG_STATIONARY | ACT_FLAG_ON_POLE | ACT_FLAG_PAUSE_EXIT)
        ACT_GRAB_POLE_SLOW             = 0x00100341, // (0x141 | ACT_FLAG_STATIONARY | ACT_FLAG_ON_POLE)
        ACT_GRAB_POLE_FAST             = 0x00100342, // (0x142 | ACT_FLAG_STATIONARY | ACT_FLAG_ON_POLE)
        ACT_CLIMBING_POLE              = 0x00100343, // (0x143 | ACT_FLAG_STATIONARY | ACT_FLAG_ON_POLE)
        ACT_TOP_OF_POLE_TRANSITION     = 0x00100344, // (0x144 | ACT_FLAG_STATIONARY | ACT_FLAG_ON_POLE)
        ACT_TOP_OF_POLE                = 0x00100345, // (0x145 | ACT_FLAG_STATIONARY | ACT_FLAG_ON_POLE)
        ACT_START_HANGING              = 0x08200348, // (0x148 | ACT_FLAG_STATIONARY | ACT_FLAG_HANGING | ACT_FLAG_PAUSE_EXIT)
        ACT_HANGING                    = 0x00200349, // (0x149 | ACT_FLAG_STATIONARY | ACT_FLAG_HANGING)
        ACT_HANG_MOVING                = 0x0020054A, // (0x14A | ACT_FLAG_MOVING | ACT_FLAG_HANGING)
        ACT_LEDGE_GRAB                 = 0x0800034B, // (0x14B | ACT_FLAG_STATIONARY | ACT_FLAG_PAUSE_EXIT)
        ACT_LEDGE_CLIMB_SLOW_1         = 0x0000054C, // (0x14C | ACT_FLAG_MOVING)
        ACT_LEDGE_CLIMB_SLOW_2         = 0x0000054D, // (0x14D | ACT_FLAG_MOVING)
        ACT_LEDGE_CLIMB_DOWN           = 0x0000054E, // (0x14E | ACT_FLAG_MOVING)
        ACT_LEDGE_CLIMB_FAST           = 0x0000054F, // (0x14F | ACT_FLAG_MOVING)
        ACT_GRABBED                    = 0x00020370, // (0x170 | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)
        ACT_IN_CANNON                  = 0x00001371, // (0x171 | ACT_FLAG_STATIONARY | ACT_FLAG_INTANGIBLE)
        ACT_TORNADO_TWIRLING           = 0x10020372, // (0x172 | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE | ACT_FLAG_SWIMMING_OR_FLYING)

    // group 0x180: object actions
        ACT_PUNCHING                   = 0x00800380, // (0x180 | ACT_FLAG_STATIONARY | ACT_FLAG_ATTACKING)
        ACT_PICKING_UP                 = 0x00000383, // (0x183 | ACT_FLAG_STATIONARY)
        ACT_DIVE_PICKING_UP            = 0x00000385, // (0x185 | ACT_FLAG_STATIONARY)
        ACT_STOMACH_SLIDE_STOP         = 0x00000386, // (0x186 | ACT_FLAG_STATIONARY)
        ACT_PLACING_DOWN               = 0x00000387, // (0x187 | ACT_FLAG_STATIONARY)
        ACT_THROWING                   = 0x80000588, // (0x188 | ACT_FLAG_MOVING | ACT_FLAG_THROWING)
        ACT_HEAVY_THROW                = 0x80000589, // (0x189 | ACT_FLAG_MOVING | ACT_FLAG_THROWING)
        ACT_PICKING_UP_BOWSER          = 0x00000390, // (0x190 | ACT_FLAG_STATIONARY)
        ACT_HOLDING_BOWSER             = 0x00000391, // (0x191 | ACT_FLAG_STATIONARY)
        ACT_RELEASING_BOWSER           = 0x00000392, // (0x192 | ACT_FLAG_STATIONARY)
    };

    enum SeqPlayer
    {
        SEQ_PLAYER_LEVEL            = 0,  // Level background music
        SEQ_PLAYER_ENV              = 1,  // Misc music like the puzzle jingle
        SEQ_PLAYER_SFX              = 2,  // Sound effects
    };

    // SeqId is from "libsm64/src/decomp/include/seq_ids.h"

    #define SEQ_VARIATION 0x80

    enum SeqId
    {
        SEQ_SOUND_PLAYER,                 // 0x00
        SEQ_EVENT_CUTSCENE_COLLECT_STAR,  // 0x01
        SEQ_MENU_TITLE_SCREEN,            // 0x02
        SEQ_LEVEL_GRASS,                  // 0x03
        SEQ_LEVEL_INSIDE_CASTLE,          // 0x04
        SEQ_LEVEL_WATER,                  // 0x05
        SEQ_LEVEL_HOT,                    // 0x06
        SEQ_LEVEL_BOSS_KOOPA,             // 0x07
        SEQ_LEVEL_SNOW,                   // 0x08
        SEQ_LEVEL_SLIDE,                  // 0x09
        SEQ_LEVEL_SPOOKY,                 // 0x0A
        SEQ_EVENT_PIRANHA_PLANT,          // 0x0B
        SEQ_LEVEL_UNDERGROUND,            // 0x0C
        SEQ_MENU_STAR_SELECT,             // 0x0D
        SEQ_EVENT_POWERUP,                // 0x0E
        SEQ_EVENT_METAL_CAP,              // 0x0F
        SEQ_EVENT_KOOPA_MESSAGE,          // 0x10
        SEQ_LEVEL_KOOPA_ROAD,             // 0x11
        SEQ_EVENT_HIGH_SCORE,             // 0x12
        SEQ_EVENT_MERRY_GO_ROUND,         // 0x13
        SEQ_EVENT_RACE,                   // 0x14
        SEQ_EVENT_CUTSCENE_STAR_SPAWN,    // 0x15
        SEQ_EVENT_BOSS,                   // 0x16
        SEQ_EVENT_CUTSCENE_COLLECT_KEY,   // 0x17
        SEQ_EVENT_ENDLESS_STAIRS,         // 0x18
        SEQ_LEVEL_BOSS_KOOPA_FINAL,       // 0x19
        SEQ_EVENT_CUTSCENE_CREDITS,       // 0x1A
        SEQ_EVENT_SOLVE_PUZZLE,           // 0x1B
        SEQ_EVENT_TOAD_MESSAGE,           // 0x1C
        SEQ_EVENT_PEACH_MESSAGE,          // 0x1D
        SEQ_EVENT_CUTSCENE_INTRO,         // 0x1E
        SEQ_EVENT_CUTSCENE_VICTORY,       // 0x1F
        SEQ_EVENT_CUTSCENE_ENDING,        // 0x20
        SEQ_MENU_FILE_SELECT,             // 0x21
        SEQ_EVENT_CUTSCENE_LAKITU,        // 0x22 (not in JP)
        SEQ_COUNT
    };

    // From "libsm64/src/decomp/game/interaction.h"
    #define INT_SUBTYPE_DELAY_INVINCIBILITY 0x00000002
    #define INT_SUBTYPE_BIG_KNOCKBACK 0x00000008 /* Used by Bowser, sets Mario's forward velocity to 40 on hit */

    void set_scale_factor(real_t p_value);
    real_t get_scale_factor() const;

    double get_tick_delta_time() const;

    // typedef void (*SM64DebugPrintFunctionPtr)( const char * );
    // extern SM64_LIB_FN void sm64_register_debug_print_function( SM64DebugPrintFunctionPtr debugPrintFunction );
    void register_debug_print_function(const godot::Callable &p_callback);
    const godot::Callable &get_debug_print_function() const;

    // typedef void (*SM64PlaySoundFunctionPtr)( uint32_t soundBits, float *pos );
    // extern SM64_LIB_FN void sm64_register_play_sound_function( SM64PlaySoundFunctionPtr playSoundFunction );
    void register_play_sound_function(const godot::Callable &p_callback);
    const godot::Callable &get_play_sound_function() const;

    // extern SM64_LIB_FN void sm64_global_init( const uint8_t *rom, uint8_t *outTexture );
    godot::Ref<godot::Image> global_init(const godot::PackedByteArray &p_rom);
    // extern SM64_LIB_FN void sm64_global_terminate( void );
    void global_terminate();

    // extern SM64_LIB_FN void sm64_audio_init( const uint8_t *rom );
    void audio_init(const godot::PackedByteArray &p_rom);
    // extern SM64_LIB_FN uint32_t sm64_audio_tick( uint32_t numQueuedSamples, uint32_t numDesiredSamples, int16_t *audio_buffer );
    godot::PackedVector2Array audio_tick(int p_queued_frames, int p_desired_frames);

    // extern SM64_LIB_FN void sm64_static_surfaces_load( const struct SM64Surface *surfaceArray, uint32_t numSurfaces );
    void static_surfaces_load(const godot::Ref<LibSM64SurfaceArray> &p_surfaces);

    // extern SM64_LIB_FN int32_t sm64_mario_create( float x, float y, float z );
    int32_t mario_create(const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_mario_tick( int32_t marioId, const struct SM64MarioInputs *inputs, struct SM64MarioState *outState, struct SM64MarioGeometryBuffers *outBuffers );
    godot::Array mario_tick(int32_t p_mario_id, const godot::Ref<LibSM64MarioInputs> &p_mario_inputs);
    // extern SM64_LIB_FN void sm64_mario_delete( int32_t marioId );
    void mario_delete(int32_t p_mario_id);

    // extern SM64_LIB_FN void sm64_set_mario_action(int32_t marioId, uint32_t action);
    void set_mario_action(int32_t p_mario_id, godot::BitField<ActionFlags> p_action);
    // extern SM64_LIB_FN void sm64_set_mario_action_arg(int32_t marioId, uint32_t action, uint32_t actionArg);
    void set_mario_action_arg(int32_t p_mario_id, godot::BitField<ActionFlags> p_action, uint32_t p_action_arg);
    // extern SM64_LIB_FN void sm64_set_mario_animation(int32_t marioId, int32_t animID);
    void set_mario_animation(int32_t p_mario_id, int32_t p_anim_id);
    // extern SM64_LIB_FN void sm64_set_mario_anim_frame(int32_t marioId, int16_t animFrame);
    void set_mario_anim_frame(int32_t p_mario_id, int16_t p_anim_frame);
    // extern SM64_LIB_FN void sm64_set_mario_state(int32_t marioId, uint32_t flags);
    void set_mario_state(int32_t p_mario_id, godot::BitField<MarioFlags> p_flags);
    // extern SM64_LIB_FN void sm64_set_mario_position(int32_t marioId, float x, float y, float z);
    void set_mario_position(int32_t p_mario_id, const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_set_mario_angle(int32_t marioId, float x, float y, float z);
    void set_mario_angle(int32_t p_mario_id, const godot::Quaternion &p_angle);
    // extern SM64_LIB_FN void sm64_set_mario_faceangle(int32_t marioId, float y);
    void set_mario_face_angle(int32_t p_mario_id, float p_y);
    // extern SM64_LIB_FN void sm64_set_mario_velocity(int32_t marioId, float x, float y, float z);
    void set_mario_velocity(int32_t p_mario_id, const godot::Vector3 &p_velocity);
    // extern SM64_LIB_FN void sm64_set_mario_forward_velocity(int32_t marioId, float vel);
    void set_mario_forward_velocity(int32_t p_mario_id, float p_velocity);
    // extern SM64_LIB_FN void sm64_set_mario_invincibility(int32_t marioId, int16_t timer);
    void set_mario_invincibility(int32_t p_mario_id, double p_time);
    // extern SM64_LIB_FN void sm64_set_mario_water_level(int32_t marioId, signed int level);
    void set_mario_water_level(int32_t p_mario_id, real_t p_level);
    // extern SM64_LIB_FN void sm64_set_mario_gas_level(int32_t marioId, signed int level);
    void set_mario_gas_level(int32_t p_mario_id, real_t p_level);
    // extern SM64_LIB_FN void sm64_set_mario_health(int32_t marioId, uint16_t health);
    void set_mario_health(int32_t p_mario_id, uint16_t p_health);
    // extern SM64_LIB_FN void sm64_mario_take_damage(int32_t marioId, uint32_t damage, uint32_t subtype, float x, float y, float z);
    void mario_take_damage(int32_t p_mario_id, uint32_t p_damage, uint32_t p_subtype, const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_mario_heal(int32_t marioId, uint8_t healCounter);
    void mario_heal(int32_t p_mario_id, uint8_t p_heal_counter);
    // extern SM64_LIB_FN void sm64_mario_kill(int32_t marioId);
    void mario_kill(int32_t p_mario_id);
    // extern SM64_LIB_FN void sm64_mario_interact_cap(int32_t marioId, uint32_t capFlag, uint16_t capTime, uint8_t playMusic);
    void mario_interact_cap(int32_t p_mario_id, godot::BitField<MarioFlags> p_cap_flag, double p_cap_time = 0.0, bool p_play_music = true);
    // extern SM64_LIB_FN void sm64_mario_extend_cap(int32_t marioId, uint16_t capTime);
    void mario_extend_cap(int32_t p_mario_id, double p_cap_time);
    // extern SM64_LIB_FN bool sm64_mario_attack(int32_t marioId, float x, float y, float z, float hitboxHeight);
    void mario_attack(int32_t p_mario_id, const godot::Vector3 &p_position, real_t p_hitbox_height);

    // extern SM64_LIB_FN uint32_t sm64_surface_object_create( const struct SM64SurfaceObject *surfaceObject );
    int surface_object_create(const godot::Vector3 &p_position, const godot::Quaternion &p_rotation, const godot::Ref<LibSM64SurfaceArray> &p_surfaces);
    // extern SM64_LIB_FN void sm64_surface_object_move( uint32_t objectId, const struct SM64ObjectTransform *transform );
    void surface_object_move(uint32_t p_object_id, const godot::Vector3 &p_position, const godot::Quaternion &p_rotation);
    // extern SM64_LIB_FN void sm64_surface_object_delete( uint32_t objectId );
    void surface_object_delete(uint32_t p_object_id);

    // extern SM64_LIB_FN int32_t sm64_surface_find_wall_collision( float *xPtr, float *yPtr, float *zPtr, float offsetY, float radius );
    // extern SM64_LIB_FN int32_t sm64_surface_find_wall_collisions( struct SM64WallCollisionData *colData );
    // extern SM64_LIB_FN float sm64_surface_find_ceil( float posX, float posY, float posZ, struct SM64SurfaceCollisionData **pceil );
    // extern SM64_LIB_FN float sm64_surface_find_floor_height_and_data( float xPos, float yPos, float zPos, struct SM64FloorCollisionData **floorGeo );
    // extern SM64_LIB_FN float sm64_surface_find_floor_height( float x, float y, float z );
    // extern SM64_LIB_FN float sm64_surface_find_floor( float xPos, float yPos, float zPos, struct SM64SurfaceCollisionData **pfloor );
    // extern SM64_LIB_FN float sm64_surface_find_water_level( float x, float z );
    // extern SM64_LIB_FN float sm64_surface_find_poison_gas_level( float x, float z );

    // extern SM64_LIB_FN void sm64_seq_player_play_sequence(uint8_t player, uint8_t seqId, uint16_t arg2);
    void seq_player_play_sequence(SeqPlayer p_player, SeqId p_seq_id, double p_fade_in_time = 0.0);
    // extern SM64_LIB_FN void sm64_play_music(uint8_t player, uint16_t seqArgs, uint16_t fadeTimer);
    void play_music(SeqPlayer p_player, uint16_t p_seq_args, double p_fade_in_time = 0.0);
    // extern SM64_LIB_FN void sm64_stop_background_music(uint16_t seqId);
    void stop_background_music(SeqId p_seq_id);
    // extern SM64_LIB_FN void sm64_fadeout_background_music(uint16_t arg0, uint16_t fadeOut);
    void fadeout_background_music(SeqId p_seq_id, double p_fade_out_time);
    // extern SM64_LIB_FN uint16_t sm64_get_current_background_music();
    uint16_t get_current_background_music();
    // extern SM64_LIB_FN void sm64_play_sound(int32_t soundBits, float *pos);
    void play_sound(int32_t p_sound_bits, const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_play_sound_global(int32_t soundBits);
    void play_sound_global(int32_t p_sound_bits);
    // extern SM64_LIB_FN void sm64_set_sound_volume(float vol);
    void set_sound_volume(float p_volume);

protected:
    static void _bind_methods();

private:
    real_t scale_factor;

    godot::Callable debug_print_function;
    godot::Callable play_sound_function;
};

VARIANT_ENUM_CAST(LibSM64::SurfaceType);
VARIANT_ENUM_CAST(LibSM64::TerrainType);

VARIANT_BITFIELD_CAST(LibSM64::ParticleFlags);
VARIANT_BITFIELD_CAST(LibSM64::MarioFlags);
VARIANT_BITFIELD_CAST(LibSM64::ActionFlags);

VARIANT_ENUM_CAST(LibSM64::SeqPlayer);
VARIANT_ENUM_CAST(LibSM64::SeqId);

#endif // LIBSM64GD_LIBSM64_H
