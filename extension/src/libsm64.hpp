#ifndef LIBSM64GD_LIBSM64_H
#define LIBSM64GD_LIBSM64_H

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/image.hpp>

#include <godot_cpp/variant/callable.hpp>

#include <sm64_surface_properties.hpp>
#include <libsm64_mario_inputs.hpp>
#include <libsm64_mario_tick_output.hpp>
#include <libsm64_surface_array.hpp>

#include <libsm64.h>

constexpr double g_sm64_timestep_interval = 1.0 / 30.0;

class LibSM64 : public godot::Object
{
    GDCLASS(LibSM64, godot::Object);

    inline static LibSM64 *singleton = nullptr;

public:
    LibSM64();
    ~LibSM64();

    static LibSM64 *get_singleton();

    void set_scale_factor(real_t p_value);
    real_t get_scale_factor() const;

    double get_sm64_timestep_interval() const;

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
    int mario_create(const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_mario_tick( int32_t marioId, const struct SM64MarioInputs *inputs, struct SM64MarioState *outState, struct SM64MarioGeometryBuffers *outBuffers );
    godot::Ref<LibSM64MarioTickOutput> mario_tick(int p_mario_id, const godot::Ref<LibSM64MarioInputs> &p_inputs);
    // extern SM64_LIB_FN void sm64_mario_delete( int32_t marioId );
    void mario_delete(int p_mario_id);

    // extern SM64_LIB_FN void sm64_set_mario_action(int32_t marioId, uint32_t action);
    void set_mario_action(int p_mario_id, int p_action);
    // extern SM64_LIB_FN void sm64_set_mario_action_arg(int32_t marioId, uint32_t action, uint32_t actionArg);
    void set_mario_action_arg(int p_mario_id, int p_action, int p_action_arg);
    // extern SM64_LIB_FN void sm64_set_mario_animation(int32_t marioId, int32_t animID);
    void set_mario_animation(int p_mario_id, int p_anim_id);
    // extern SM64_LIB_FN void sm64_set_mario_anim_frame(int32_t marioId, int16_t animFrame);
    void set_mario_anim_frame(int p_mario_id, int p_anim_frame);
    // extern SM64_LIB_FN void sm64_set_mario_state(int32_t marioId, uint32_t flags);
    void set_mario_state(int p_mario_id, int p_flags);
    // extern SM64_LIB_FN void sm64_set_mario_position(int32_t marioId, float x, float y, float z);
    void set_mario_position(int p_mario_id, const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_set_mario_angle(int32_t marioId, float x, float y, float z);
    void set_mario_angle(int p_mario_id, const godot::Quaternion &p_angle);
    // extern SM64_LIB_FN void sm64_set_mario_faceangle(int32_t marioId, float y);
    void set_mario_face_angle(int p_mario_id, float p_y);
    // extern SM64_LIB_FN void sm64_set_mario_velocity(int32_t marioId, float x, float y, float z);
    void set_mario_velocity(int p_mario_id, const godot::Vector3 &p_velocity);
    // extern SM64_LIB_FN void sm64_set_mario_forward_velocity(int32_t marioId, float vel);
    void set_mario_forward_velocity(int p_mario_id, float p_velocity);
    // extern SM64_LIB_FN void sm64_set_mario_invincibility(int32_t marioId, int16_t timer);
    void set_mario_invincibility(int p_mario_id, double p_time);
    // extern SM64_LIB_FN void sm64_set_mario_water_level(int32_t marioId, signed int level);
    void set_mario_water_level(int p_mario_id, real_t p_level);
    // extern SM64_LIB_FN void sm64_set_mario_gas_level(int32_t marioId, signed int level);
    void set_mario_gas_level(int p_mario_id, real_t p_level);
    // extern SM64_LIB_FN void sm64_set_mario_health(int32_t marioId, uint16_t health);
    void set_mario_health(int p_mario_id, int p_health);
    // extern SM64_LIB_FN void sm64_mario_take_damage(int32_t marioId, uint32_t damage, uint32_t subtype, float x, float y, float z);
    void mario_take_damage(int p_mario_id, int p_damage, int p_subtype, const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_mario_heal(int32_t marioId, uint8_t healCounter);
    void mario_heal(int p_mario_id, int p_heal_counter);
    // extern SM64_LIB_FN void sm64_mario_kill(int32_t marioId);
    void mario_kill(int p_mario_id);
    // extern SM64_LIB_FN void sm64_mario_interact_cap(int32_t marioId, uint32_t capFlag, uint16_t capTime, uint8_t playMusic);
    void mario_interact_cap(int p_mario_id, int p_cap_flag, int p_cap_time, bool p_play_music);
    // extern SM64_LIB_FN void sm64_mario_extend_cap(int32_t marioId, uint16_t capTime);
    void mario_extend_cap(int p_mario_id, double p_cap_time);
    // extern SM64_LIB_FN bool sm64_mario_attack(int32_t marioId, float x, float y, float z, float hitboxHeight);
    void mario_attack(int p_mario_id, const godot::Vector3 &p_position, float p_hitbox_height);

    // extern SM64_LIB_FN uint32_t sm64_surface_object_create( const struct SM64SurfaceObject *surfaceObject );
    int surface_object_create(const godot::Vector3 &p_position, const godot::Quaternion &p_rotation, const godot::Ref<LibSM64SurfaceArray> &p_surfaces);
    // extern SM64_LIB_FN void sm64_surface_object_move( uint32_t objectId, const struct SM64ObjectTransform *transform );
    void surface_object_move(int p_object_id, const godot::Vector3 &p_position, const godot::Quaternion &p_rotation);
    // extern SM64_LIB_FN void sm64_surface_object_delete( uint32_t objectId );
    void surface_object_delete(int p_object_id);

    // extern SM64_LIB_FN int32_t sm64_surface_find_wall_collision( float *xPtr, float *yPtr, float *zPtr, float offsetY, float radius );
    // extern SM64_LIB_FN int32_t sm64_surface_find_wall_collisions( struct SM64WallCollisionData *colData );
    // extern SM64_LIB_FN float sm64_surface_find_ceil( float posX, float posY, float posZ, struct SM64SurfaceCollisionData **pceil );
    // extern SM64_LIB_FN float sm64_surface_find_floor_height_and_data( float xPos, float yPos, float zPos, struct SM64FloorCollisionData **floorGeo );
    // extern SM64_LIB_FN float sm64_surface_find_floor_height( float x, float y, float z );
    // extern SM64_LIB_FN float sm64_surface_find_floor( float xPos, float yPos, float zPos, struct SM64SurfaceCollisionData **pfloor );
    // extern SM64_LIB_FN float sm64_surface_find_water_level( float x, float z );
    // extern SM64_LIB_FN float sm64_surface_find_poison_gas_level( float x, float z );

    // extern SM64_LIB_FN void sm64_seq_player_play_sequence(uint8_t player, uint8_t seqId, uint16_t arg2);
    void seq_player_play_sequence(int p_player, int p_seq_id, double p_fade_in_time);
    // extern SM64_LIB_FN void sm64_play_music(uint8_t player, uint16_t seqArgs, uint16_t fadeTimer);
    void play_music(int p_player, int p_seq_args, double p_fade_in_time);
    // extern SM64_LIB_FN void sm64_stop_background_music(uint16_t seqId);
    void stop_background_music(int p_seq_id);
    // extern SM64_LIB_FN void sm64_fadeout_background_music(uint16_t arg0, uint16_t fadeOut);
    void fadeout_background_music(int p_seq_id, double p_fade_out_time);
    // extern SM64_LIB_FN uint16_t sm64_get_current_background_music();
    int get_current_background_music();
    // extern SM64_LIB_FN void sm64_play_sound(int32_t soundBits, float *pos);
    void play_sound(int p_sound_bits, const godot::Vector3 &p_position);
    // extern SM64_LIB_FN void sm64_play_sound_global(int32_t soundBits);
    void play_sound_global(int p_sound_bits);
    // extern SM64_LIB_FN void sm64_set_sound_volume(float vol);
    void set_sound_volume(float p_volume);

protected:
    static void _bind_methods();

private:
    real_t scale_factor = 75.0;

    godot::Callable debug_print_function;
    godot::Callable play_sound_function;
};

#endif // LIBSM64GD_LIBSM64_H
