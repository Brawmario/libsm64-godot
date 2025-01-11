#include <libsm64.hpp>

#include <godot_cpp/classes/array_mesh.hpp>

#include <libsm64_mario_geometry.hpp>
#include <libsm64_mario_state.hpp>

// SM64 3D vector to Godot 3D vector
// Winding order: counter-clockwise (SM64) -> clockwise (Godot)
// SM64 vector(x, y, z) -> Godot vector(-z, y, x)

static _FORCE_INLINE_ void godot_to_sm64(const godot::Vector3 &p_vec, float *p_arr_out, real_t p_scale_factor)
{
    p_arr_out[0] = p_vec.z / p_scale_factor;
    p_arr_out[1] = p_vec.y / p_scale_factor;
    p_arr_out[2] = -p_vec.x / p_scale_factor;
}

static _FORCE_INLINE_ void godot_to_sm64(const godot::Vector3 &p_vec, float *p_arr_out)
{
    p_arr_out[0] = p_vec.z;
    p_arr_out[1] = p_vec.y;
    p_arr_out[2] = -p_vec.x;
}

static _FORCE_INLINE_ void godot_to_sm64(const godot::Vector3 &p_vec, float &p_x_out, float &p_y_out, float &p_z_out, real_t p_scale_factor)
{
    p_x_out = p_vec.z / p_scale_factor;
    p_y_out = p_vec.y / p_scale_factor;
    p_z_out = -p_vec.x / p_scale_factor;
}

static _FORCE_INLINE_ void godot_to_sm64(const godot::Vector3 &p_vec, float &p_x_out, float &p_y_out, float &p_z_out)
{
    p_x_out = p_vec.z;
    p_y_out = p_vec.y;
    p_z_out = -p_vec.x;
}

static _FORCE_INLINE_ void godot_to_sm64_angle(const godot::Quaternion &p_rotation, float *p_arr_out)
{
    godot::Vector3 rotation_vec = p_rotation.get_euler_yxz();

    p_arr_out[0] = -godot::Math::rad_to_deg(rotation_vec.z);
    p_arr_out[1] = -godot::Math::rad_to_deg(rotation_vec.y);
    p_arr_out[2] = godot::Math::rad_to_deg(rotation_vec.x);
}

static _FORCE_INLINE_ void godot_to_sm64_angle(const godot::Quaternion &p_rotation, float &p_x_out, float &p_y_out, float &p_z_out)
{
    godot::Vector3 rotation_vec = p_rotation.get_euler_yxz();

    p_x_out = -godot::Math::rad_to_deg(rotation_vec.z);
    p_y_out = -godot::Math::rad_to_deg(rotation_vec.y);
    p_z_out =  godot::Math::rad_to_deg(rotation_vec.x);
}

static _FORCE_INLINE_ godot::Vector3 sm64_3d_to_godot(const float *p_arr)
{
    return godot::Vector3(-p_arr[2], p_arr[1], p_arr[0]);
}

static _FORCE_INLINE_ void sm64_3d_to_godot(const float *sm64_vec_arr, godot::Vector3 *godot_vec_arr, int vertex_count, real_t scale_factor)
{
    for (int i = 0; i < vertex_count; i += 3)
    {
        godot::Vector3 *godot_vec = &godot_vec_arr[i];
        const float *sm64_vec = &sm64_vec_arr[3 * i];

        godot_vec[0] = sm64_3d_to_godot(&sm64_vec[3 * 1]) / scale_factor;
        godot_vec[1] = sm64_3d_to_godot(&sm64_vec[3 * 0]) / scale_factor;
        godot_vec[2] = sm64_3d_to_godot(&sm64_vec[3 * 2]) / scale_factor;
    }
}

static _FORCE_INLINE_ void sm64_3d_to_godot(const float *sm64_vec_arr, godot::Vector3 *godot_vec_arr, int vertex_count)
{
    for (int i = 0; i < vertex_count; i += 3)
    {
        godot::Vector3 *godot_vec = &godot_vec_arr[i];
        const float *sm64_vec = &sm64_vec_arr[3 * i];

        godot_vec[0] = sm64_3d_to_godot(&sm64_vec[3 * 1]);
        godot_vec[1] = sm64_3d_to_godot(&sm64_vec[3 * 0]);
        godot_vec[2] = sm64_3d_to_godot(&sm64_vec[3 * 2]);
    }
}

// SM64 color to Godot color
// Winding order: counter-clockwise (SM64) -> clockwise (Godot)
// SM64 color(r, g, b) -> Godot color(r, g, b, a)

static _FORCE_INLINE_ godot::Color sm64_color_to_godot(const float *sm64_color, float alpha = 1.0f)
{
    return godot::Color(sm64_color[0], sm64_color[1], sm64_color[2], alpha);
}

static _FORCE_INLINE_ void sm64_color_to_godot(const float *sm64_color_arr, godot::Color *godot_color_arr, int vertex_count, bool wing_cap_on)
{
    for (int i = 0; i < vertex_count; i += 3)
    {
        godot::Color *godot_color = &godot_color_arr[i];
        const float *sm64_color = &sm64_color_arr[3 * i];

        // Add transparency to the wings of the wing cap (last 24 vertexes)
        const float alpha = (wing_cap_on && vertex_count > 2256 && i >= vertex_count - 24) ? 0.0f : 1.0f;
        godot_color[0] = sm64_color_to_godot(&sm64_color[3 * 1], alpha);
        godot_color[1] = sm64_color_to_godot(&sm64_color[3 * 0], alpha);
        godot_color[2] = sm64_color_to_godot(&sm64_color[3 * 2], alpha);
    }
}

static _FORCE_INLINE_ godot::Vector2 sm64_2d_to_godot(const float *p_arr)
{
    return godot::Vector2(p_arr[0], p_arr[1]);
}

static _FORCE_INLINE_ void sm64_2d_to_godot(const float *sm64_vec_arr, godot::Vector2 *godot_vec_arr, int vertex_count)
{
    for (int i = 0; i < vertex_count; i += 3)
    {
        godot::Vector2 *godot_vec = &godot_vec_arr[i];
        const float *sm64_vec = &sm64_vec_arr[2 * i];

        godot_vec[0] = sm64_2d_to_godot(&sm64_vec[2 * 1]);
        godot_vec[1] = sm64_2d_to_godot(&sm64_vec[2 * 0]);
        godot_vec[2] = sm64_2d_to_godot(&sm64_vec[2 * 2]);
    }
}

static void SM64DebugPrintFunction(const char *msg)
{
    LibSM64 *libsm64 = LibSM64::get_singleton();
    ERR_FAIL_NULL(libsm64);

    const godot::Callable &debug_print_function = libsm64->get_debug_print_function();
    if (unlikely(!debug_print_function.is_valid()))
    {
        ERR_PRINT("[libsm64-godot] debug print function is not valid");
        sm64_register_debug_print_function(NULL);
        return;
    }

    debug_print_function.call(msg);
}

static void SM64PlaySoundFunction(uint32_t soundBits, float *pos)
{
    LibSM64 *libsm64 = LibSM64::get_singleton();
    ERR_FAIL_NULL(libsm64);

    const godot::Callable &play_sound_function = libsm64->get_play_sound_function();
    if (unlikely(!play_sound_function.is_valid()))
    {
        ERR_PRINT("[libsm64-godot] play sound function is not valid");
        sm64_register_play_sound_function(NULL);
        return;
    }

    play_sound_function.call(soundBits, sm64_3d_to_godot(pos));
}

LibSM64::LibSM64()
{
    ERR_FAIL_COND(singleton != nullptr);
    singleton = this;
}

LibSM64::~LibSM64()
{
    ERR_FAIL_COND(singleton != this);
    singleton = nullptr;

    global_terminate();

    sm64_register_debug_print_function(NULL);
    sm64_register_play_sound_function(NULL);
}

LibSM64 *LibSM64::get_singleton()
{
    return singleton;
}

void LibSM64::set_scale_factor(real_t p_value)
{
    scale_factor = p_value;
}

real_t LibSM64::get_scale_factor() const
{
    return scale_factor;
}

double LibSM64::get_sm64_timestep_interval() const
{
    return g_sm64_timestep_interval;
}

void LibSM64::register_debug_print_function(const godot::Callable &p_callback)
{
    debug_print_function = p_callback;
    sm64_register_debug_print_function(SM64DebugPrintFunction);
}

const godot::Callable &LibSM64::get_debug_print_function() const
{
    return debug_print_function;
}

void LibSM64::register_play_sound_function(const godot::Callable &p_callback)
{
    play_sound_function = p_callback;
    sm64_register_play_sound_function(SM64PlaySoundFunction);
}

const godot::Callable &LibSM64::get_play_sound_function() const
{
    return play_sound_function;
}

godot::Ref<godot::Image> LibSM64::global_init(const godot::PackedByteArray &p_rom)
{
    godot::PackedByteArray mario_image_bytes;
    mario_image_bytes.resize(4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT);

    sm64_global_init(p_rom.ptr(), mario_image_bytes.ptrw());

    return godot::Image::create_from_data(SM64_TEXTURE_WIDTH, SM64_TEXTURE_HEIGHT, false, godot::Image::FORMAT_RGBA8, mario_image_bytes);
}

void LibSM64::global_terminate()
{
    sm64_global_terminate();
}

void LibSM64::audio_init(const godot::PackedByteArray &p_rom)
{
    sm64_audio_init(p_rom.ptr());
}

godot::PackedVector2Array LibSM64::audio_tick(int p_queued_frames, int p_desired_frames)
{
    godot::PackedVector2Array frames;

    std::array<int16_t, 544 * 2 * 2> audio_buffer;
    const int num_queded_samples = p_queued_frames / 2;
    const int num_desired_samples = p_desired_frames / 2;
    const int num_audio_samples = sm64_audio_tick(num_queded_samples, num_desired_samples, audio_buffer.data());
    const int frame_count = num_audio_samples * 2;

    frames.resize(frame_count);
    godot::Vector2 *frames_ptrw = frames.ptrw();
    for (int i = 0; i < frame_count; i++)
    {
        auto &frame = frames_ptrw[i];
        auto *audio_buffer_frame_pair = &audio_buffer[2 * i];
        frame.x = real_t(audio_buffer_frame_pair[0]) / 32767.0;
        frame.y = real_t(audio_buffer_frame_pair[1]) / 32767.0;
    }

    return frames;
}

void LibSM64::static_surfaces_load(const godot::Ref<LibSM64SurfaceArray> &p_surfaces)
{
    ERR_FAIL_NULL(p_surfaces);

    sm64_static_surfaces_load(p_surfaces->sm64_surfaces.data(), p_surfaces->sm64_surfaces.size());
}

int LibSM64::mario_create(const godot::Vector3 &p_position)
{
    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);

    int mario_id = sm64_mario_create(x, y, z);
    ERR_FAIL_COND_V_MSG(mario_id < 0, mario_id, "[libsm64-godot] Failed to create Mario. Have you created a floor for him to stand on yet?");

    return mario_id;
}

godot::Ref<LibSM64MarioTickOutput> LibSM64::mario_tick(int p_mario_id, const godot::Ref<LibSM64MarioInputs> &p_inputs)
{
    ERR_FAIL_COND_V(p_mario_id < 0, nullptr);
    ERR_FAIL_NULL_V(p_inputs, nullptr);

    const struct SM64MarioInputs sm64_mario_inputs = p_inputs->to_sm64();
    struct SM64MarioState sm64_mario_state;
    LibSM64MarioGeometry sm64_mario_geometry;
    sm64_mario_tick(p_mario_id, &sm64_mario_inputs, &sm64_mario_state, sm64_mario_geometry.data());

    godot::Ref<LibSM64MarioState> mario_state = memnew(LibSM64MarioState(sm64_mario_state));

    const int vertex_count = sm64_mario_geometry.triangles() * 3;
    godot::PackedVector3Array position;
    position.resize(vertex_count);
    godot::PackedVector3Array normal;
    normal.resize(vertex_count);
    godot::PackedColorArray color;
    color.resize(vertex_count);
    godot::PackedVector2Array uv;
    uv.resize(vertex_count);

    sm64_3d_to_godot(sm64_mario_geometry.position.data(), position.ptrw(), vertex_count);
    sm64_3d_to_godot(sm64_mario_geometry.normal.data(), normal.ptrw(), vertex_count);
    sm64_color_to_godot(sm64_mario_geometry.color.data(), color.ptrw(), vertex_count, sm64_mario_state.flags & 0x8);
    sm64_2d_to_godot(sm64_mario_geometry.uv.data(), uv.ptrw(), vertex_count);

    godot::Array array_mesh_triangles;
    array_mesh_triangles.resize(godot::ArrayMesh::ARRAY_MAX);
    array_mesh_triangles[godot::ArrayMesh::ARRAY_VERTEX] = position;
    array_mesh_triangles[godot::ArrayMesh::ARRAY_NORMAL] = normal;
    array_mesh_triangles[godot::ArrayMesh::ARRAY_COLOR]  = color;
    array_mesh_triangles[godot::ArrayMesh::ARRAY_TEX_UV] = uv;

    return memnew(LibSM64MarioTickOutput(mario_state, array_mesh_triangles, -mario_state->get_position(), godot::Vector3(1.0 / scale_factor, 1.0 / scale_factor, 1.0 / scale_factor)));
}

void LibSM64::mario_delete(int p_mario_id)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_delete(p_mario_id);
}

void LibSM64::set_mario_action(int p_mario_id, int p_action)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_action(p_mario_id, p_action);
}

void LibSM64::set_mario_action_arg(int p_mario_id, int p_action, int p_action_arg)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_action_arg(p_mario_id, p_action, p_action_arg);
}

void LibSM64::set_mario_animation(int p_mario_id, int p_anim_id)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_animation(p_mario_id, p_anim_id);
}

void LibSM64::set_mario_anim_frame(int p_mario_id, int p_anim_frame)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_anim_frame(p_mario_id, p_anim_frame);
}

void LibSM64::set_mario_state(int p_mario_id, int p_flags)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_state(p_mario_id, p_flags);
}

void LibSM64::set_mario_position(int p_mario_id, const godot::Vector3 &p_position)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);
    sm64_set_mario_position(p_mario_id, x, y, z);
}

void LibSM64::set_mario_angle(int p_mario_id, const godot::Quaternion &p_angle)
{
    ERR_FAIL_COND(p_mario_id < 0);

    godot::Vector3 rotation_vec = p_angle.get_euler_yxz();
    sm64_set_mario_angle(p_mario_id, rotation_vec.x, rotation_vec.y, rotation_vec.z);
}

void LibSM64::set_mario_face_angle(int p_mario_id, float p_y)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_faceangle(p_mario_id, p_y);
}

void LibSM64::set_mario_velocity(int p_mario_id, const godot::Vector3 &p_velocity)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_velocity, x, y, z, scale_factor / g_sm64_timestep_interval);
    sm64_set_mario_velocity(p_mario_id, x, y, z);
}

void LibSM64::set_mario_forward_velocity(int p_mario_id, float p_velocity)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_forward_velocity(p_mario_id, p_velocity / (scale_factor * g_sm64_timestep_interval));
}

void LibSM64::set_mario_invincibility(int p_mario_id, double p_time)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_invincibility(p_mario_id, int16_t(p_time / g_sm64_timestep_interval));
}

void LibSM64::set_mario_water_level(int p_mario_id, real_t p_level)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_water_level(p_mario_id, int(p_level / scale_factor));
}

void LibSM64::set_mario_gas_level(int p_mario_id, real_t p_level)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_gas_level(p_mario_id, int(p_level / scale_factor));
}

void LibSM64::set_mario_health(int p_mario_id, int p_health)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_health(p_mario_id, p_health);
}

void LibSM64::mario_take_damage(int p_mario_id, int p_damage, int p_subtype, const godot::Vector3 &p_position)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);
    sm64_mario_take_damage(p_mario_id, p_damage, p_subtype, x, y, z);
}

void LibSM64::mario_heal(int p_mario_id, int p_heal_counter)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_heal(p_mario_id, p_heal_counter);
}

void LibSM64::mario_kill(int p_mario_id)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_kill(p_mario_id);
}

void LibSM64::mario_interact_cap(int p_mario_id, int p_cap_flag, int p_cap_time, bool p_play_music)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_interact_cap(p_mario_id, p_cap_flag, p_cap_time, p_play_music);
}

void LibSM64::mario_extend_cap(int p_mario_id, double p_cap_time)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_extend_cap(p_mario_id, uint16_t(p_cap_time / g_sm64_timestep_interval));
}

void LibSM64::mario_attack(int p_mario_id, const godot::Vector3 &p_position, float p_hitbox_height)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);
    sm64_mario_attack(p_mario_id, x, y, z, p_hitbox_height / scale_factor);
}

int LibSM64::surface_object_create(const godot::Vector3 &p_position, const godot::Quaternion &p_rotation, const godot::Ref<LibSM64SurfaceArray> &p_surfaces)
{
    ERR_FAIL_NULL_V(p_surfaces, -1);

    struct SM64ObjectTransform object_transform;
    godot_to_sm64(p_position, object_transform.position, scale_factor);
    godot_to_sm64_angle(p_rotation, object_transform.eulerRotation);

    const struct SM64SurfaceObject object = {
        object_transform,
        static_cast<uint32_t>(p_surfaces->sm64_surfaces.size()),
        const_cast<SM64Surface*>(p_surfaces->sm64_surfaces.data())
    };

    return sm64_surface_object_create(&object);
}

void LibSM64::surface_object_move(int p_object_id, const godot::Vector3 &p_position, const godot::Quaternion &p_rotation)
{
    ERR_FAIL_COND(p_object_id < 0);

    struct SM64ObjectTransform object_transform;
    godot_to_sm64(p_position, object_transform.position, scale_factor);
    godot_to_sm64_angle(p_rotation, object_transform.eulerRotation);

    sm64_surface_object_move(p_object_id, &object_transform);
}

void LibSM64::surface_object_delete(int p_object_id)
{
    sm64_surface_object_delete(p_object_id);
}

void LibSM64::seq_player_play_sequence(int p_player, int p_seq_id, double p_fade_in_time)
{
    sm64_seq_player_play_sequence(p_player, p_seq_id, p_fade_in_time / g_sm64_timestep_interval);
}

void LibSM64::play_music(int p_player, int p_seq_args, double p_fade_in_time)
{
    sm64_play_music(p_player, p_seq_args, p_fade_in_time / g_sm64_timestep_interval);
}

void LibSM64::stop_background_music(int p_seq_id)
{
    sm64_stop_background_music(p_seq_id);
}

void LibSM64::fadeout_background_music(int p_seq_id, double p_fade_out_time)
{
    sm64_fadeout_background_music(p_seq_id, p_fade_out_time / g_sm64_timestep_interval);
}

void LibSM64::play_sound(int p_sound_bits, const godot::Vector3 &p_position)
{
    float pos[3];
    godot_to_sm64(p_position, pos, scale_factor);
    sm64_play_sound(p_sound_bits, pos);
}

void LibSM64::play_sound_global(int p_sound_bits)
{
    sm64_play_sound_global(p_sound_bits);
}

void LibSM64::set_sound_volume(float p_volume)
{
    sm64_set_sound_volume(p_volume);
}

void LibSM64::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_scale_factor", "value"), &LibSM64::set_scale_factor);
    godot::ClassDB::bind_method(godot::D_METHOD("get_scale_factor"), &LibSM64::get_scale_factor);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "scale_factor"), "set_scale_factor", "get_scale_factor");

    godot::ClassDB::bind_method(godot::D_METHOD("get_sm64_timestep_interval"), &LibSM64::get_sm64_timestep_interval);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "sm64_timestep_interval"), "", "get_sm64_timestep_interval");

    godot::ClassDB::bind_method(godot::D_METHOD("register_debug_print_function", "callback"), &LibSM64::register_debug_print_function);
    godot::ClassDB::bind_method(godot::D_METHOD("register_play_sound_function", "callback"), &LibSM64::register_play_sound_function);

    godot::ClassDB::bind_method(godot::D_METHOD("global_init", "rom"), &LibSM64::global_init);
    godot::ClassDB::bind_method(godot::D_METHOD("global_terminate"), &LibSM64::global_terminate);

    godot::ClassDB::bind_method(godot::D_METHOD("audio_init", "rom"), &LibSM64::audio_init);
    godot::ClassDB::bind_method(godot::D_METHOD("audio_tick", "queued_frames", "desired_frames"), &LibSM64::audio_tick);

    godot::ClassDB::bind_method(godot::D_METHOD("static_surfaces_load", "surfaces"), &LibSM64::static_surfaces_load);

    godot::ClassDB::bind_method(godot::D_METHOD("mario_create", "position"), &LibSM64::mario_create);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_tick", "mario_id", "inputs"), &LibSM64::mario_tick);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_delete", "mario_id"), &LibSM64::mario_delete);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_action", "mario_id", "action"), &LibSM64::set_mario_action);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_action_arg", "mario_id", "action", "action_arg"), &LibSM64::set_mario_action_arg);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_animation", "mario_id", "anim_id"), &LibSM64::set_mario_animation);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_anim_frame", "mario_id", "anim_frame"), &LibSM64::set_mario_anim_frame);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_state", "mario_id", "flags"), &LibSM64::set_mario_state);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_position", "mario_id", "position"), &LibSM64::set_mario_position);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_angle", "mario_id", "angle"), &LibSM64::set_mario_angle);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_face_angle", "mario_id", "y"), &LibSM64::set_mario_face_angle);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_velocity", "mario_id", "velocity"), &LibSM64::set_mario_velocity);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_forward_velocity", "mario_id", "velocity"), &LibSM64::set_mario_forward_velocity);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_invincibility", "mario_id", "time"), &LibSM64::set_mario_invincibility);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_water_level", "mario_id", "level"), &LibSM64::set_mario_water_level);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_gas_level", "mario_id", "level"), &LibSM64::set_mario_gas_level);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_health", "mario_id", "health"), &LibSM64::set_mario_health);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_take_damage", "mario_id", "damage", "subtype", "position"), &LibSM64::mario_take_damage);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_heal", "mario_id", "heal_counter"), &LibSM64::mario_heal);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_kill", "mario_id"), &LibSM64::mario_kill);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_interact_cap", "mario_id", "cap_flag", "cap_time", "play_music"), &LibSM64::mario_interact_cap);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_extend_cap", "mario_id", "cap_time"), &LibSM64::mario_extend_cap);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_attack", "mario_id", "position", "hitbox_height"), &LibSM64::mario_attack);

    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_create", "position", "rotation", "surfaces"), &LibSM64::surface_object_create);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_move", "object_id", "position", "rotation"), &LibSM64::surface_object_move);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_delete", "object_id"), &LibSM64::surface_object_delete);

    godot::ClassDB::bind_method(godot::D_METHOD("seq_player_play_sequence", "player", "seq_id", "fade_in_time"), &LibSM64::seq_player_play_sequence);
    godot::ClassDB::bind_method(godot::D_METHOD("play_music", "player", "seq_args", "fade_in_time"), &LibSM64::play_music);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_background_music", "seq_id"), &LibSM64::stop_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("fadeout_background_music", "seq_id", "fade_out_time"), &LibSM64::fadeout_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("play_sound", "sound_bits", "position"), &LibSM64::play_sound);
    godot::ClassDB::bind_method(godot::D_METHOD("play_sound_global", "sound_bits"), &LibSM64::play_sound_global);
    godot::ClassDB::bind_method(godot::D_METHOD("set_sound_volume", "volume"), &LibSM64::set_sound_volume);
}
