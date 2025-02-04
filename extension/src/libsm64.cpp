#include <libsm64.hpp>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/project_settings.hpp>

#include <libsm64_mario_geometry.hpp>
#include <libsm64_mario_inputs.hpp>
#include <libsm64_mario_state.hpp>
#include <libsm64_surface_array.hpp>

constexpr char SCALE_FACTOR_SETTING_NAME[] = "libsm64/scale_factor";

// SM64 3D vector to Godot 3D vector
// Winding order: counter-clockwise (SM64) -> clockwise (Godot)
// SM64 vector(x, y, z) -> Godot vector(-z, y, x)

static _FORCE_INLINE_ void godot_to_sm64(const godot::Vector3 &p_vec, float *p_arr_out, real_t p_scale_factor)
{
    p_arr_out[0] =  p_vec.z * p_scale_factor;
    p_arr_out[1] =  p_vec.y * p_scale_factor;
    p_arr_out[2] = -p_vec.x * p_scale_factor;
}

static _FORCE_INLINE_ void godot_to_sm64(const godot::Vector3 &p_vec, float &p_x_out, float &p_y_out, float &p_z_out, real_t p_scale_factor)
{
    p_x_out =  p_vec.z * p_scale_factor;
    p_y_out =  p_vec.y * p_scale_factor;
    p_z_out = -p_vec.x * p_scale_factor;
}

static _FORCE_INLINE_ void godot_to_sm64_mario_angle(const godot::Quaternion &p_rotation, float &p_x_out, float &p_y_out, float &p_z_out)
{
    godot::Vector3 euler_rot = godot::Basis(p_rotation).get_euler(godot::EULER_ORDER_YZX);

    p_x_out =  euler_rot.z;
    p_y_out =  euler_rot.y;
    p_z_out = -euler_rot.x;
}

static _FORCE_INLINE_ void godot_to_sm64_object_rotation(const godot::Quaternion &p_rotation, float *p_arr_out)
{
    godot::Vector3 euler_rot = godot::Basis(p_rotation).get_euler(godot::EULER_ORDER_YZX);

    p_arr_out[0] = -godot::Math::rad_to_deg(euler_rot.z);
    p_arr_out[1] = -godot::Math::rad_to_deg(euler_rot.y);
    p_arr_out[2] =  godot::Math::rad_to_deg(euler_rot.x);
}

static _FORCE_INLINE_ godot::Vector3 sm64_3d_to_godot(const float *p_arr)
{
    return godot::Vector3(-p_arr[2], p_arr[1], p_arr[0]);
}

static _FORCE_INLINE_ godot::Vector3 sm64_3d_to_godot(const float *p_arr, real_t scale_factor)
{
    return godot::Vector3(-p_arr[2] / scale_factor, p_arr[1] / scale_factor, p_arr[0] / scale_factor);
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

static _FORCE_INLINE_ void sm64_3d_to_godot(const float *sm64_vec_arr, godot::Vector3 *godot_vec_arr, int vertex_count, real_t scale_factor)
{
    for (int i = 0; i < vertex_count; i += 3)
    {
        godot::Vector3 *godot_vec = &godot_vec_arr[i];
        const float *sm64_vec = &sm64_vec_arr[3 * i];

        godot_vec[0] = sm64_3d_to_godot(&sm64_vec[3 * 1], scale_factor);
        godot_vec[1] = sm64_3d_to_godot(&sm64_vec[3 * 0], scale_factor);
        godot_vec[2] = sm64_3d_to_godot(&sm64_vec[3 * 2], scale_factor);
    }
}

// SM64 color to Godot color
// Winding order: counter-clockwise (SM64) -> clockwise (Godot)
// SM64 color(r, g, b) -> Godot color(r, g, b, a)

static _FORCE_INLINE_ godot::Color sm64_color_to_godot(const float *sm64_color, float alpha)
{
    return godot::Color(sm64_color[0], sm64_color[1], sm64_color[2], alpha);
}

static _FORCE_INLINE_ void sm64_color_to_godot(const float *sm64_color_arr, godot::Color *godot_color_arr, int vertex_count, bool wing_cap_on)
{
    for (int i = 0; i < vertex_count; i += 3)
    {
        godot::Color *godot_color = &godot_color_arr[i];
        const float *sm64_color = &sm64_color_arr[3 * i];

        // Add transparency to the wings of the wing cap (last 24 vertices)
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

// Project setting functions adapted from Godot Jolt addon

static void register_project_setting(const char *p_name, godot::Variant::Type p_type, const godot::Variant &p_default_value)
{
    auto *project_settings = godot::ProjectSettings::get_singleton();

    if (!project_settings->has_setting(p_name))
    {
        project_settings->set(p_name, p_default_value);
    }

    auto setting = godot::Dictionary();
    setting["name"] = p_name;
    setting["type"] = p_type;

    project_settings->add_property_info(setting);
    project_settings->set_initial_value(p_name, p_default_value);
}

template<typename T>
static T get_project_setting(const char *p_name, T p_default_value = {})
{
    const auto* project_settings = godot::ProjectSettings::get_singleton();

    const godot::Variant setting_value = project_settings->get_setting_with_override(p_name);
    const godot::Variant::Type setting_type = setting_value.get_type();
    const godot::Variant::Type expected_type = godot::Variant(T()).get_type();

    ERR_FAIL_COND_V_MSG(
        setting_type != expected_type,
        p_default_value,
        godot::vformat(
            "Unexpected type for setting '%s'. Expected type '%s' but found '%s'.",
            p_name,
            godot::Variant::get_type_name(expected_type),
            godot::Variant::get_type_name(setting_type)
        )
    );

    return setting_value;
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

    scale_factor = get_project_setting<real_t>(SCALE_FACTOR_SETTING_NAME, 100.0);
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

void LibSM64::register_project_settings()
{
    register_project_setting(SCALE_FACTOR_SETTING_NAME, godot::Variant::FLOAT, 100.0);
}

void LibSM64::set_scale_factor(real_t p_value)
{
    scale_factor = p_value;
}

real_t LibSM64::get_scale_factor() const
{
    return scale_factor;
}

double LibSM64::get_tick_delta_time() const
{
    return tick_delta_time;
}

void LibSM64::register_debug_print_function(const godot::Callable &p_callback)
{
    debug_print_function = p_callback;
    sm64_register_debug_print_function(p_callback.is_null() ? NULL : SM64DebugPrintFunction);
}

const godot::Callable &LibSM64::get_debug_print_function() const
{
    return debug_print_function;
}

void LibSM64::register_play_sound_function(const godot::Callable &p_callback)
{
    play_sound_function = p_callback;
    sm64_register_play_sound_function(p_callback.is_null() ? NULL : SM64PlaySoundFunction);
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
        frame.x = static_cast<real_t>(audio_buffer_frame_pair[0]) / 32767.0;
        frame.y = static_cast<real_t>(audio_buffer_frame_pair[1]) / 32767.0;
    }

    return frames;
}

void LibSM64::static_surfaces_load(const godot::Ref<LibSM64SurfaceArray> &p_surfaces)
{
    ERR_FAIL_NULL(p_surfaces);

    sm64_static_surfaces_load(p_surfaces->sm64_surfaces.data(), p_surfaces->sm64_surfaces.size());
}

int32_t LibSM64::mario_create(const godot::Vector3 &p_position)
{
    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);

    int32_t mario_id = sm64_mario_create(x, y, z);
    ERR_FAIL_COND_V_MSG(mario_id < 0, mario_id, "[libsm64-godot] Failed to create Mario. Have you created a floor for him to stand on yet?");

    return mario_id;
}

godot::Array LibSM64::mario_tick(int32_t p_mario_id, const godot::Ref<LibSM64MarioInputs> &p_mario_inputs)
{
    godot::Array ret;

    ERR_FAIL_COND_V(p_mario_id < 0, ret);
    ERR_FAIL_NULL_V(p_mario_inputs, ret);

    const struct SM64MarioInputs sm64_mario_inputs = p_mario_inputs->to_sm64();
    struct SM64MarioState sm64_mario_state;
    LibSM64MarioGeometry sm64_mario_geometry;
    sm64_mario_tick(p_mario_id, &sm64_mario_inputs, &sm64_mario_state, sm64_mario_geometry.data());

    godot::Ref<LibSM64MarioState> mario_state = memnew(LibSM64MarioState(sm64_mario_state, scale_factor));

    const int vertex_count = sm64_mario_geometry.triangles() * 3;
    godot::PackedVector3Array position;
    position.resize(vertex_count);
    godot::PackedVector3Array normal;
    normal.resize(vertex_count);
    godot::PackedColorArray color;
    color.resize(vertex_count);
    godot::PackedVector2Array uv;
    uv.resize(vertex_count);

    sm64_3d_to_godot(sm64_mario_geometry.position.data(), position.ptrw(), vertex_count, scale_factor);
    sm64_3d_to_godot(sm64_mario_geometry.normal.data(), normal.ptrw(), vertex_count);
    sm64_color_to_godot(sm64_mario_geometry.color.data(), color.ptrw(), vertex_count, sm64_mario_state.flags & MARIO_WING_CAP);
    sm64_2d_to_godot(sm64_mario_geometry.uv.data(), uv.ptrw(), vertex_count);

    godot::Array array_mesh_triangles;
    array_mesh_triangles.resize(godot::ArrayMesh::ARRAY_MAX);
    array_mesh_triangles[godot::ArrayMesh::ARRAY_VERTEX] = position;
    array_mesh_triangles[godot::ArrayMesh::ARRAY_NORMAL] = normal;
    array_mesh_triangles[godot::ArrayMesh::ARRAY_COLOR]  = color;
    array_mesh_triangles[godot::ArrayMesh::ARRAY_TEX_UV] = uv;

    ret.append(mario_state);
    ret.append(array_mesh_triangles);

    return ret;
}

void LibSM64::mario_delete(int32_t p_mario_id)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_delete(p_mario_id);
}

void LibSM64::set_mario_action(int32_t p_mario_id, godot::BitField<ActionFlags> p_action)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_action(p_mario_id, static_cast<uint32_t>(p_action));
}

void LibSM64::set_mario_action_arg(int32_t p_mario_id, godot::BitField<ActionFlags> p_action, uint32_t p_action_arg)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_action_arg(p_mario_id, static_cast<uint32_t>(p_action), p_action_arg);
}

void LibSM64::set_mario_animation(int32_t p_mario_id, int32_t p_anim_id)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_animation(p_mario_id, p_anim_id);
}

void LibSM64::set_mario_anim_frame(int32_t p_mario_id, int16_t p_anim_frame)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_anim_frame(p_mario_id, p_anim_frame);
}

void LibSM64::set_mario_state(int32_t p_mario_id, godot::BitField<MarioFlags> p_flags)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_state(p_mario_id, static_cast<uint32_t>(p_flags));
}

void LibSM64::set_mario_position(int32_t p_mario_id, const godot::Vector3 &p_position)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);
    sm64_set_mario_position(p_mario_id, x, y, z);
}

void LibSM64::set_mario_angle(int32_t p_mario_id, const godot::Quaternion &p_angle)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64_mario_angle(p_angle, x, y, z);
    sm64_set_mario_angle(p_mario_id, x, y, z);
}

void LibSM64::set_mario_face_angle(int32_t p_mario_id, float p_y)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_faceangle(p_mario_id, p_y);
}

void LibSM64::set_mario_velocity(int32_t p_mario_id, const godot::Vector3 &p_velocity)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_velocity, x, y, z, scale_factor * tick_delta_time);
    sm64_set_mario_velocity(p_mario_id, x, y, z);
}

void LibSM64::set_mario_forward_velocity(int32_t p_mario_id, float p_velocity)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_forward_velocity(p_mario_id, p_velocity * scale_factor * tick_delta_time);
}

void LibSM64::set_mario_invincibility(int32_t p_mario_id, double p_time)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_invincibility(p_mario_id, static_cast<int16_t>(p_time / tick_delta_time));
}

void LibSM64::set_mario_water_level(int32_t p_mario_id, real_t p_level)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_water_level(p_mario_id, static_cast<signed int>(p_level * scale_factor));
}

void LibSM64::set_mario_gas_level(int32_t p_mario_id, real_t p_level)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_gas_level(p_mario_id, static_cast<signed int>(p_level * scale_factor));
}

void LibSM64::set_mario_health(int32_t p_mario_id, uint16_t p_health)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_set_mario_health(p_mario_id, p_health);
}

void LibSM64::mario_take_damage(int32_t p_mario_id, uint32_t p_damage, uint32_t p_subtype, const godot::Vector3 &p_position)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);
    sm64_mario_take_damage(p_mario_id, p_damage, p_subtype, x, y, z);
}

void LibSM64::mario_heal(int32_t p_mario_id, uint8_t p_heal_counter)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_heal(p_mario_id, p_heal_counter);
}

void LibSM64::mario_kill(int32_t p_mario_id)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_kill(p_mario_id);
}

void LibSM64::mario_interact_cap(int32_t p_mario_id, godot::BitField<MarioFlags> p_cap_flag, double p_cap_time, bool p_play_music)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_interact_cap(p_mario_id, static_cast<uint32_t>(p_cap_flag), static_cast<uint16_t>(p_cap_time / tick_delta_time), static_cast<uint8_t>(p_play_music));
}

void LibSM64::mario_extend_cap(int32_t p_mario_id, double p_cap_time)
{
    ERR_FAIL_COND(p_mario_id < 0);

    sm64_mario_extend_cap(p_mario_id, static_cast<uint16_t>(p_cap_time / tick_delta_time));
}

void LibSM64::mario_attack(int32_t p_mario_id, const godot::Vector3 &p_position, real_t p_hitbox_height)
{
    ERR_FAIL_COND(p_mario_id < 0);

    float x, y, z;
    godot_to_sm64(p_position, x, y, z, scale_factor);
    sm64_mario_attack(p_mario_id, x, y, z, p_hitbox_height * scale_factor);
}

int LibSM64::surface_object_create(const godot::Vector3 &p_position, const godot::Quaternion &p_rotation, const godot::Ref<LibSM64SurfaceArray> &p_surfaces)
{
    ERR_FAIL_NULL_V(p_surfaces, -1);

    struct SM64ObjectTransform object_transform;
    godot_to_sm64(p_position, object_transform.position, scale_factor);
    godot_to_sm64_object_rotation(p_rotation, object_transform.eulerRotation);

    const struct SM64SurfaceObject object = {
        object_transform,
        static_cast<uint32_t>(p_surfaces->sm64_surfaces.size()),
        const_cast<SM64Surface*>(p_surfaces->sm64_surfaces.data())
    };

    return sm64_surface_object_create(&object);
}

void LibSM64::surface_object_move(uint32_t p_object_id, const godot::Vector3 &p_position, const godot::Quaternion &p_rotation)
{
    ERR_FAIL_COND(p_object_id < 0);

    struct SM64ObjectTransform object_transform;
    godot_to_sm64(p_position, object_transform.position, scale_factor);
    godot_to_sm64_object_rotation(p_rotation, object_transform.eulerRotation);

    sm64_surface_object_move(p_object_id, &object_transform);
}

void LibSM64::surface_object_delete(uint32_t p_object_id)
{
    sm64_surface_object_delete(p_object_id);
}

void LibSM64::seq_player_play_sequence(SeqPlayer p_player, SeqId p_seq_id, double p_fade_in_time)
{
    sm64_seq_player_play_sequence(static_cast<uint8_t>(p_player), static_cast<uint8_t>(p_seq_id), p_fade_in_time / tick_delta_time);
}

void LibSM64::play_music(SeqPlayer p_player, uint16_t p_seq_args, double p_fade_in_time)
{
    sm64_play_music(static_cast<uint8_t>(p_player), p_seq_args, p_fade_in_time / tick_delta_time);
}

void LibSM64::stop_background_music(SeqId p_seq_id)
{
    sm64_stop_background_music(static_cast<uint16_t>(p_seq_id));
}

void LibSM64::fadeout_background_music(SeqId p_seq_id, double p_fade_out_time)
{
    sm64_fadeout_background_music(static_cast<uint16_t>(p_seq_id), p_fade_out_time / tick_delta_time);
}

uint16_t LibSM64::get_current_background_music()
{
    return sm64_get_current_background_music();
}

void LibSM64::play_sound(godot::BitField<SoundBits> p_sound_bits, const godot::Vector3 &p_position)
{
    float pos[3];
    godot_to_sm64(p_position, pos, scale_factor);
    sm64_play_sound(static_cast<uint32_t>(p_sound_bits), pos);
}

void LibSM64::play_sound_global(godot::BitField<SoundBits> p_sound_bits)
{
    sm64_play_sound_global(static_cast<uint32_t>(p_sound_bits));
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

    godot::ClassDB::bind_method(godot::D_METHOD("get_tick_delta_time"), &LibSM64::get_tick_delta_time);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "tick_delta_time"), "", "get_tick_delta_time");

    godot::ClassDB::bind_method(godot::D_METHOD("register_debug_print_function", "callback"), &LibSM64::register_debug_print_function);
    godot::ClassDB::bind_method(godot::D_METHOD("register_play_sound_function", "callback"), &LibSM64::register_play_sound_function);

    godot::ClassDB::bind_method(godot::D_METHOD("global_init", "rom"), &LibSM64::global_init);
    godot::ClassDB::bind_method(godot::D_METHOD("global_terminate"), &LibSM64::global_terminate);

    godot::ClassDB::bind_method(godot::D_METHOD("audio_init", "rom"), &LibSM64::audio_init);
    godot::ClassDB::bind_method(godot::D_METHOD("audio_tick", "queued_frames", "desired_frames"), &LibSM64::audio_tick);

    godot::ClassDB::bind_method(godot::D_METHOD("static_surfaces_load", "surfaces"), &LibSM64::static_surfaces_load);

    godot::ClassDB::bind_method(godot::D_METHOD("mario_create", "position"), &LibSM64::mario_create);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_tick", "mario_id", "mario_inputs"), &LibSM64::mario_tick);
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
    godot::ClassDB::bind_method(godot::D_METHOD("mario_interact_cap", "mario_id", "cap_flag", "cap_time", "play_music"), &LibSM64::mario_interact_cap, DEFVAL(0.0), DEFVAL(true));
    godot::ClassDB::bind_method(godot::D_METHOD("mario_extend_cap", "mario_id", "cap_time"), &LibSM64::mario_extend_cap);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_attack", "mario_id", "position", "hitbox_height"), &LibSM64::mario_attack);

    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_create", "position", "rotation", "surfaces"), &LibSM64::surface_object_create);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_move", "object_id", "position", "rotation"), &LibSM64::surface_object_move);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_delete", "object_id"), &LibSM64::surface_object_delete);

    godot::ClassDB::bind_method(godot::D_METHOD("seq_player_play_sequence", "player", "seq_id", "fade_in_time"), &LibSM64::seq_player_play_sequence, DEFVAL(0.0));
    godot::ClassDB::bind_method(godot::D_METHOD("play_music", "player", "seq_args", "fade_in_time"), &LibSM64::play_music, DEFVAL(0.0));
    godot::ClassDB::bind_method(godot::D_METHOD("stop_background_music", "seq_id"), &LibSM64::stop_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("fadeout_background_music", "seq_id", "fade_out_time"), &LibSM64::fadeout_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("get_current_background_music"), &LibSM64::get_current_background_music);
    godot::ClassDB::bind_method(godot::D_METHOD("play_sound", "sound_bits", "position"), &LibSM64::play_sound);
    godot::ClassDB::bind_method(godot::D_METHOD("play_sound_global", "sound_bits"), &LibSM64::play_sound_global);
    godot::ClassDB::bind_method(godot::D_METHOD("set_sound_volume", "volume"), &LibSM64::set_sound_volume);

    BIND_ENUM_CONSTANT(SURFACE_DEFAULT);
    BIND_ENUM_CONSTANT(SURFACE_BURNING);
    BIND_ENUM_CONSTANT(SURFACE_0004);
    BIND_ENUM_CONSTANT(SURFACE_HANGABLE);
    BIND_ENUM_CONSTANT(SURFACE_SLOW);
    BIND_ENUM_CONSTANT(SURFACE_DEATH_PLANE);
    BIND_ENUM_CONSTANT(SURFACE_CLOSE_CAMERA);
    BIND_ENUM_CONSTANT(SURFACE_WATER);
    BIND_ENUM_CONSTANT(SURFACE_FLOWING_WATER);
    BIND_ENUM_CONSTANT(SURFACE_INTANGIBLE);
    BIND_ENUM_CONSTANT(SURFACE_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_NOT_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TTM_VINES);
    BIND_ENUM_CONSTANT(SURFACE_MGR_MUSIC);
    BIND_ENUM_CONSTANT(SURFACE_INSTANT_WARP_1B);
    BIND_ENUM_CONSTANT(SURFACE_INSTANT_WARP_1C);
    BIND_ENUM_CONSTANT(SURFACE_INSTANT_WARP_1D);
    BIND_ENUM_CONSTANT(SURFACE_INSTANT_WARP_1E);
    BIND_ENUM_CONSTANT(SURFACE_SHALLOW_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_DEEP_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_INSTANT_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_DEEP_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_SHALLOW_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_WALL_MISC);
    BIND_ENUM_CONSTANT(SURFACE_NOISE_DEFAULT);
    BIND_ENUM_CONSTANT(SURFACE_NOISE_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_HORIZONTAL_WIND);
    BIND_ENUM_CONSTANT(SURFACE_INSTANT_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_ICE);
    BIND_ENUM_CONSTANT(SURFACE_LOOK_UP_WARP);
    BIND_ENUM_CONSTANT(SURFACE_HARD);
    BIND_ENUM_CONSTANT(SURFACE_WARP);
    BIND_ENUM_CONSTANT(SURFACE_TIMER_START);
    BIND_ENUM_CONSTANT(SURFACE_TIMER_END);
    BIND_ENUM_CONSTANT(SURFACE_HARD_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_HARD_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_HARD_NOT_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_VERTICAL_WIND);
    BIND_ENUM_CONSTANT(SURFACE_BOSS_FIGHT_CAMERA);
    BIND_ENUM_CONSTANT(SURFACE_CAMERA_FREE_ROAM);
    BIND_ENUM_CONSTANT(SURFACE_THI3_WALLKICK);
    BIND_ENUM_CONSTANT(SURFACE_CAMERA_8_DIR);
    BIND_ENUM_CONSTANT(SURFACE_CAMERA_MIDDLE);
    BIND_ENUM_CONSTANT(SURFACE_CAMERA_ROTATE_RIGHT);
    BIND_ENUM_CONSTANT(SURFACE_CAMERA_ROTATE_LEFT);
    BIND_ENUM_CONSTANT(SURFACE_CAMERA_BOUNDARY);
    BIND_ENUM_CONSTANT(SURFACE_NOISE_VERY_SLIPPERY_73);
    BIND_ENUM_CONSTANT(SURFACE_NOISE_VERY_SLIPPERY_74);
    BIND_ENUM_CONSTANT(SURFACE_NOISE_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_NO_CAM_COLLISION);
    BIND_ENUM_CONSTANT(SURFACE_NO_CAM_COLLISION_77);
    BIND_ENUM_CONSTANT(SURFACE_NO_CAM_COL_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_NO_CAM_COL_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_SWITCH);
    BIND_ENUM_CONSTANT(SURFACE_VANISH_CAP_WALLS);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_A6);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_A7);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_A8);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_A9);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_AA);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_AB);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_AC);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_AD);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_AE);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_AF);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B0);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B1);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B2);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B3);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B4);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B5);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B6);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B7);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B8);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_B9);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_BA);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_BB);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_BC);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_BD);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_BE);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_BF);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C0);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C1);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C2);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C3);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C4);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C5);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C6);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C7);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C8);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_C9);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_CA);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_CB);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_CC);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_CD);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_CE);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_CF);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_D0);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_D1);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WOBBLE_D2);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_D3);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_D4);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_D5);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_D6);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_D7);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_D8);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_D9);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_DA);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_DB);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_DC);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_DD);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_DE);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_DF);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E0);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E1);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E2);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E3);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E4);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E5);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E6);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E7);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E8);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_E9);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_EA);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_EB);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_EC);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_ED);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_EE);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_EF);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_F0);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_F1);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_F2);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_F3);
    BIND_ENUM_CONSTANT(SURFACE_TTC_PAINTING_1);
    BIND_ENUM_CONSTANT(SURFACE_TTC_PAINTING_2);
    BIND_ENUM_CONSTANT(SURFACE_TTC_PAINTING_3);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_F7);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_F8);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_F9);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_FA);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_FB);
    BIND_ENUM_CONSTANT(SURFACE_PAINTING_WARP_FC);
    BIND_ENUM_CONSTANT(SURFACE_WOBBLING_WARP);
    BIND_ENUM_CONSTANT(SURFACE_TRAPDOOR);

    BIND_ENUM_CONSTANT(TERRAIN_GRASS);
    BIND_ENUM_CONSTANT(TERRAIN_STONE);
    BIND_ENUM_CONSTANT(TERRAIN_SNOW);
    BIND_ENUM_CONSTANT(TERRAIN_SAND);
    BIND_ENUM_CONSTANT(TERRAIN_SPOOKY);
    BIND_ENUM_CONSTANT(TERRAIN_WATER);
    BIND_ENUM_CONSTANT(TERRAIN_SLIDE);
    BIND_ENUM_CONSTANT(TERRAIN_MASK);

    BIND_BITFIELD_FLAG(PARTICLE_DUST);
    BIND_BITFIELD_FLAG(PARTICLE_VERTICAL_STAR);
    BIND_BITFIELD_FLAG(PARTICLE_2);
    BIND_BITFIELD_FLAG(PARTICLE_SPARKLES);
    BIND_BITFIELD_FLAG(PARTICLE_HORIZONTAL_STAR);
    BIND_BITFIELD_FLAG(PARTICLE_BUBBLE);
    BIND_BITFIELD_FLAG(PARTICLE_WATER_SPLASH);
    BIND_BITFIELD_FLAG(PARTICLE_IDLE_WATER_WAVE);
    BIND_BITFIELD_FLAG(PARTICLE_SHALLOW_WATER_WAVE);
    BIND_BITFIELD_FLAG(PARTICLE_PLUNGE_BUBBLE);
    BIND_BITFIELD_FLAG(PARTICLE_WAVE_TRAIL);
    BIND_BITFIELD_FLAG(PARTICLE_FIRE);
    BIND_BITFIELD_FLAG(PARTICLE_SHALLOW_WATER_SPLASH);
    BIND_BITFIELD_FLAG(PARTICLE_LEAF);
    BIND_BITFIELD_FLAG(PARTICLE_SNOW);
    BIND_BITFIELD_FLAG(PARTICLE_DIRT);
    BIND_BITFIELD_FLAG(PARTICLE_MIST_CIRCLE);
    BIND_BITFIELD_FLAG(PARTICLE_BREATH);
    BIND_BITFIELD_FLAG(PARTICLE_TRIANGLE);
    BIND_BITFIELD_FLAG(PARTICLE_19);

    BIND_BITFIELD_FLAG(MARIO_NORMAL_CAP);
    BIND_BITFIELD_FLAG(MARIO_VANISH_CAP);
    BIND_BITFIELD_FLAG(MARIO_METAL_CAP);
    BIND_BITFIELD_FLAG(MARIO_WING_CAP);
    BIND_BITFIELD_FLAG(MARIO_CAP_ON_HEAD);
    BIND_BITFIELD_FLAG(MARIO_CAP_IN_HAND);
    BIND_BITFIELD_FLAG(MARIO_METAL_SHOCK);
    BIND_BITFIELD_FLAG(MARIO_TELEPORTING);
    BIND_BITFIELD_FLAG(MARIO_UNKNOWN_08);
    BIND_BITFIELD_FLAG(MARIO_UNKNOWN_13);
    BIND_BITFIELD_FLAG(MARIO_ACTION_SOUND_PLAYED);
    BIND_BITFIELD_FLAG(MARIO_MARIO_SOUND_PLAYED);
    BIND_BITFIELD_FLAG(MARIO_UNKNOWN_18);
    BIND_BITFIELD_FLAG(MARIO_PUNCHING);
    BIND_BITFIELD_FLAG(MARIO_KICKING);
    BIND_BITFIELD_FLAG(MARIO_TRIPPING);
    BIND_BITFIELD_FLAG(MARIO_UNKNOWN_25);
    BIND_BITFIELD_FLAG(MARIO_UNKNOWN_30);
    BIND_BITFIELD_FLAG(MARIO_UNKNOWN_31);
    BIND_BITFIELD_FLAG(MARIO_SPECIAL_CAPS);
    BIND_BITFIELD_FLAG(MARIO_CAPS);

    BIND_BITFIELD_FLAG(ACT_ID_MASK);
    BIND_BITFIELD_FLAG(ACT_GROUP_MASK);
    BIND_BITFIELD_FLAG(ACT_GROUP_STATIONARY);
    BIND_BITFIELD_FLAG(ACT_GROUP_MOVING);
    BIND_BITFIELD_FLAG(ACT_GROUP_AIRBORNE);
    BIND_BITFIELD_FLAG(ACT_GROUP_SUBMERGED);
    BIND_BITFIELD_FLAG(ACT_GROUP_CUTSCENE);
    BIND_BITFIELD_FLAG(ACT_GROUP_AUTOMATIC);
    BIND_BITFIELD_FLAG(ACT_GROUP_OBJECT);
    BIND_BITFIELD_FLAG(ACT_FLAG_STATIONARY);
    BIND_BITFIELD_FLAG(ACT_FLAG_MOVING);
    BIND_BITFIELD_FLAG(ACT_FLAG_AIR);
    BIND_BITFIELD_FLAG(ACT_FLAG_INTANGIBLE);
    BIND_BITFIELD_FLAG(ACT_FLAG_SWIMMING);
    BIND_BITFIELD_FLAG(ACT_FLAG_METAL_WATER);
    BIND_BITFIELD_FLAG(ACT_FLAG_SHORT_HITBOX);
    BIND_BITFIELD_FLAG(ACT_FLAG_RIDING_SHELL);
    BIND_BITFIELD_FLAG(ACT_FLAG_INVULNERABLE);
    BIND_BITFIELD_FLAG(ACT_FLAG_BUTT_OR_STOMACH_SLIDE);
    BIND_BITFIELD_FLAG(ACT_FLAG_DIVING);
    BIND_BITFIELD_FLAG(ACT_FLAG_ON_POLE);
    BIND_BITFIELD_FLAG(ACT_FLAG_HANGING);
    BIND_BITFIELD_FLAG(ACT_FLAG_IDLE);
    BIND_BITFIELD_FLAG(ACT_FLAG_ATTACKING);
    BIND_BITFIELD_FLAG(ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION);
    BIND_BITFIELD_FLAG(ACT_FLAG_CONTROL_JUMP_HEIGHT);
    BIND_BITFIELD_FLAG(ACT_FLAG_ALLOW_FIRST_PERSON);
    BIND_BITFIELD_FLAG(ACT_FLAG_PAUSE_EXIT);
    BIND_BITFIELD_FLAG(ACT_FLAG_SWIMMING_OR_FLYING);
    BIND_BITFIELD_FLAG(ACT_FLAG_WATER_OR_TEXT);
    BIND_BITFIELD_FLAG(ACT_FLAG_THROWING);
    BIND_BITFIELD_FLAG(ACT_UNINITIALIZED);
    BIND_BITFIELD_FLAG(ACT_IDLE);
    BIND_BITFIELD_FLAG(ACT_START_SLEEPING);
    BIND_BITFIELD_FLAG(ACT_SLEEPING);
    BIND_BITFIELD_FLAG(ACT_WAKING_UP);
    BIND_BITFIELD_FLAG(ACT_PANTING);
    BIND_BITFIELD_FLAG(ACT_HOLD_PANTING_UNUSED);
    BIND_BITFIELD_FLAG(ACT_HOLD_IDLE);
    BIND_BITFIELD_FLAG(ACT_HOLD_HEAVY_IDLE);
    BIND_BITFIELD_FLAG(ACT_STANDING_AGAINST_WALL);
    BIND_BITFIELD_FLAG(ACT_COUGHING);
    BIND_BITFIELD_FLAG(ACT_SHIVERING);
    BIND_BITFIELD_FLAG(ACT_IN_QUICKSAND);
    BIND_BITFIELD_FLAG(ACT_UNKNOWN_0002020E);
    BIND_BITFIELD_FLAG(ACT_CROUCHING);
    BIND_BITFIELD_FLAG(ACT_START_CROUCHING);
    BIND_BITFIELD_FLAG(ACT_STOP_CROUCHING);
    BIND_BITFIELD_FLAG(ACT_START_CRAWLING);
    BIND_BITFIELD_FLAG(ACT_STOP_CRAWLING);
    BIND_BITFIELD_FLAG(ACT_SLIDE_KICK_SLIDE_STOP);
    BIND_BITFIELD_FLAG(ACT_SHOCKWAVE_BOUNCE);
    BIND_BITFIELD_FLAG(ACT_FIRST_PERSON);
    BIND_BITFIELD_FLAG(ACT_BACKFLIP_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_JUMP_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_DOUBLE_JUMP_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_FREEFALL_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_SIDE_FLIP_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_HOLD_JUMP_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_HOLD_FREEFALL_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_AIR_THROW_LAND);
    BIND_BITFIELD_FLAG(ACT_TWIRL_LAND);
    BIND_BITFIELD_FLAG(ACT_LAVA_BOOST_LAND);
    BIND_BITFIELD_FLAG(ACT_TRIPLE_JUMP_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_LONG_JUMP_LAND_STOP);
    BIND_BITFIELD_FLAG(ACT_GROUND_POUND_LAND);
    BIND_BITFIELD_FLAG(ACT_BRAKING_STOP);
    BIND_BITFIELD_FLAG(ACT_BUTT_SLIDE_STOP);
    BIND_BITFIELD_FLAG(ACT_HOLD_BUTT_SLIDE_STOP);
    BIND_BITFIELD_FLAG(ACT_WALKING);
    BIND_BITFIELD_FLAG(ACT_HOLD_WALKING);
    BIND_BITFIELD_FLAG(ACT_TURNING_AROUND);
    BIND_BITFIELD_FLAG(ACT_FINISH_TURNING_AROUND);
    BIND_BITFIELD_FLAG(ACT_BRAKING);
    BIND_BITFIELD_FLAG(ACT_RIDING_SHELL_GROUND);
    BIND_BITFIELD_FLAG(ACT_HOLD_HEAVY_WALKING);
    BIND_BITFIELD_FLAG(ACT_CRAWLING);
    BIND_BITFIELD_FLAG(ACT_BURNING_GROUND);
    BIND_BITFIELD_FLAG(ACT_DECELERATING);
    BIND_BITFIELD_FLAG(ACT_HOLD_DECELERATING);
    BIND_BITFIELD_FLAG(ACT_BEGIN_SLIDING);
    BIND_BITFIELD_FLAG(ACT_HOLD_BEGIN_SLIDING);
    BIND_BITFIELD_FLAG(ACT_BUTT_SLIDE);
    BIND_BITFIELD_FLAG(ACT_STOMACH_SLIDE);
    BIND_BITFIELD_FLAG(ACT_HOLD_BUTT_SLIDE);
    BIND_BITFIELD_FLAG(ACT_HOLD_STOMACH_SLIDE);
    BIND_BITFIELD_FLAG(ACT_DIVE_SLIDE);
    BIND_BITFIELD_FLAG(ACT_MOVE_PUNCHING);
    BIND_BITFIELD_FLAG(ACT_CROUCH_SLIDE);
    BIND_BITFIELD_FLAG(ACT_SLIDE_KICK_SLIDE);
    BIND_BITFIELD_FLAG(ACT_HARD_BACKWARD_GROUND_KB);
    BIND_BITFIELD_FLAG(ACT_HARD_FORWARD_GROUND_KB);
    BIND_BITFIELD_FLAG(ACT_BACKWARD_GROUND_KB);
    BIND_BITFIELD_FLAG(ACT_FORWARD_GROUND_KB);
    BIND_BITFIELD_FLAG(ACT_SOFT_BACKWARD_GROUND_KB);
    BIND_BITFIELD_FLAG(ACT_SOFT_FORWARD_GROUND_KB);
    BIND_BITFIELD_FLAG(ACT_GROUND_BONK);
    BIND_BITFIELD_FLAG(ACT_DEATH_EXIT_LAND);
    BIND_BITFIELD_FLAG(ACT_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_FREEFALL_LAND);
    BIND_BITFIELD_FLAG(ACT_DOUBLE_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_SIDE_FLIP_LAND);
    BIND_BITFIELD_FLAG(ACT_HOLD_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_HOLD_FREEFALL_LAND);
    BIND_BITFIELD_FLAG(ACT_QUICKSAND_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_HOLD_QUICKSAND_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_TRIPLE_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_LONG_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_BACKFLIP_LAND);
    BIND_BITFIELD_FLAG(ACT_JUMP);
    BIND_BITFIELD_FLAG(ACT_DOUBLE_JUMP);
    BIND_BITFIELD_FLAG(ACT_TRIPLE_JUMP);
    BIND_BITFIELD_FLAG(ACT_BACKFLIP);
    BIND_BITFIELD_FLAG(ACT_STEEP_JUMP);
    BIND_BITFIELD_FLAG(ACT_WALL_KICK_AIR);
    BIND_BITFIELD_FLAG(ACT_SIDE_FLIP);
    BIND_BITFIELD_FLAG(ACT_LONG_JUMP);
    BIND_BITFIELD_FLAG(ACT_WATER_JUMP);
    BIND_BITFIELD_FLAG(ACT_DIVE);
    BIND_BITFIELD_FLAG(ACT_FREEFALL);
    BIND_BITFIELD_FLAG(ACT_TOP_OF_POLE_JUMP);
    BIND_BITFIELD_FLAG(ACT_BUTT_SLIDE_AIR);
    BIND_BITFIELD_FLAG(ACT_FLYING_TRIPLE_JUMP);
    BIND_BITFIELD_FLAG(ACT_SHOT_FROM_CANNON);
    BIND_BITFIELD_FLAG(ACT_FLYING);
    BIND_BITFIELD_FLAG(ACT_RIDING_SHELL_JUMP);
    BIND_BITFIELD_FLAG(ACT_RIDING_SHELL_FALL);
    BIND_BITFIELD_FLAG(ACT_VERTICAL_WIND);
    BIND_BITFIELD_FLAG(ACT_HOLD_JUMP);
    BIND_BITFIELD_FLAG(ACT_HOLD_FREEFALL);
    BIND_BITFIELD_FLAG(ACT_HOLD_BUTT_SLIDE_AIR);
    BIND_BITFIELD_FLAG(ACT_HOLD_WATER_JUMP);
    BIND_BITFIELD_FLAG(ACT_TWIRLING);
    BIND_BITFIELD_FLAG(ACT_FORWARD_ROLLOUT);
    BIND_BITFIELD_FLAG(ACT_AIR_HIT_WALL);
    BIND_BITFIELD_FLAG(ACT_RIDING_HOOT);
    BIND_BITFIELD_FLAG(ACT_GROUND_POUND);
    BIND_BITFIELD_FLAG(ACT_SLIDE_KICK);
    BIND_BITFIELD_FLAG(ACT_AIR_THROW);
    BIND_BITFIELD_FLAG(ACT_JUMP_KICK);
    BIND_BITFIELD_FLAG(ACT_BACKWARD_ROLLOUT);
    BIND_BITFIELD_FLAG(ACT_CRAZY_BOX_BOUNCE);
    BIND_BITFIELD_FLAG(ACT_SPECIAL_TRIPLE_JUMP);
    BIND_BITFIELD_FLAG(ACT_BACKWARD_AIR_KB);
    BIND_BITFIELD_FLAG(ACT_FORWARD_AIR_KB);
    BIND_BITFIELD_FLAG(ACT_HARD_FORWARD_AIR_KB);
    BIND_BITFIELD_FLAG(ACT_HARD_BACKWARD_AIR_KB);
    BIND_BITFIELD_FLAG(ACT_BURNING_JUMP);
    BIND_BITFIELD_FLAG(ACT_BURNING_FALL);
    BIND_BITFIELD_FLAG(ACT_SOFT_BONK);
    BIND_BITFIELD_FLAG(ACT_LAVA_BOOST);
    BIND_BITFIELD_FLAG(ACT_GETTING_BLOWN);
    BIND_BITFIELD_FLAG(ACT_THROWN_FORWARD);
    BIND_BITFIELD_FLAG(ACT_THROWN_BACKWARD);
    BIND_BITFIELD_FLAG(ACT_WATER_IDLE);
    BIND_BITFIELD_FLAG(ACT_HOLD_WATER_IDLE);
    BIND_BITFIELD_FLAG(ACT_WATER_ACTION_END);
    BIND_BITFIELD_FLAG(ACT_HOLD_WATER_ACTION_END);
    BIND_BITFIELD_FLAG(ACT_DROWNING);
    BIND_BITFIELD_FLAG(ACT_BACKWARD_WATER_KB);
    BIND_BITFIELD_FLAG(ACT_FORWARD_WATER_KB);
    BIND_BITFIELD_FLAG(ACT_WATER_DEATH);
    BIND_BITFIELD_FLAG(ACT_WATER_SHOCKED);
    BIND_BITFIELD_FLAG(ACT_BREASTSTROKE);
    BIND_BITFIELD_FLAG(ACT_SWIMMING_END);
    BIND_BITFIELD_FLAG(ACT_FLUTTER_KICK);
    BIND_BITFIELD_FLAG(ACT_HOLD_BREASTSTROKE);
    BIND_BITFIELD_FLAG(ACT_HOLD_SWIMMING_END);
    BIND_BITFIELD_FLAG(ACT_HOLD_FLUTTER_KICK);
    BIND_BITFIELD_FLAG(ACT_WATER_SHELL_SWIMMING);
    BIND_BITFIELD_FLAG(ACT_WATER_THROW);
    BIND_BITFIELD_FLAG(ACT_WATER_PUNCH);
    BIND_BITFIELD_FLAG(ACT_WATER_PLUNGE);
    BIND_BITFIELD_FLAG(ACT_CAUGHT_IN_WHIRLPOOL);
    BIND_BITFIELD_FLAG(ACT_METAL_WATER_STANDING);
    BIND_BITFIELD_FLAG(ACT_HOLD_METAL_WATER_STANDING);
    BIND_BITFIELD_FLAG(ACT_METAL_WATER_WALKING);
    BIND_BITFIELD_FLAG(ACT_HOLD_METAL_WATER_WALKING);
    BIND_BITFIELD_FLAG(ACT_METAL_WATER_FALLING);
    BIND_BITFIELD_FLAG(ACT_HOLD_METAL_WATER_FALLING);
    BIND_BITFIELD_FLAG(ACT_METAL_WATER_FALL_LAND);
    BIND_BITFIELD_FLAG(ACT_HOLD_METAL_WATER_FALL_LAND);
    BIND_BITFIELD_FLAG(ACT_METAL_WATER_JUMP);
    BIND_BITFIELD_FLAG(ACT_HOLD_METAL_WATER_JUMP);
    BIND_BITFIELD_FLAG(ACT_METAL_WATER_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_HOLD_METAL_WATER_JUMP_LAND);
    BIND_BITFIELD_FLAG(ACT_DISAPPEARED);
    BIND_BITFIELD_FLAG(ACT_INTRO_CUTSCENE);
    BIND_BITFIELD_FLAG(ACT_STAR_DANCE_EXIT);
    BIND_BITFIELD_FLAG(ACT_STAR_DANCE_WATER);
    BIND_BITFIELD_FLAG(ACT_FALL_AFTER_STAR_GRAB);
    BIND_BITFIELD_FLAG(ACT_READING_AUTOMATIC_DIALOG);
    BIND_BITFIELD_FLAG(ACT_READING_NPC_DIALOG);
    BIND_BITFIELD_FLAG(ACT_STAR_DANCE_NO_EXIT);
    BIND_BITFIELD_FLAG(ACT_READING_SIGN);
    BIND_BITFIELD_FLAG(ACT_JUMBO_STAR_CUTSCENE);
    BIND_BITFIELD_FLAG(ACT_WAITING_FOR_DIALOG);
    BIND_BITFIELD_FLAG(ACT_DEBUG_FREE_MOVE);
    BIND_BITFIELD_FLAG(ACT_STANDING_DEATH);
    BIND_BITFIELD_FLAG(ACT_QUICKSAND_DEATH);
    BIND_BITFIELD_FLAG(ACT_ELECTROCUTION);
    BIND_BITFIELD_FLAG(ACT_SUFFOCATION);
    BIND_BITFIELD_FLAG(ACT_DEATH_ON_STOMACH);
    BIND_BITFIELD_FLAG(ACT_DEATH_ON_BACK);
    BIND_BITFIELD_FLAG(ACT_EATEN_BY_BUBBA);
    BIND_BITFIELD_FLAG(ACT_END_PEACH_CUTSCENE);
    BIND_BITFIELD_FLAG(ACT_CREDITS_CUTSCENE);
    BIND_BITFIELD_FLAG(ACT_END_WAVING_CUTSCENE);
    BIND_BITFIELD_FLAG(ACT_PULLING_DOOR);
    BIND_BITFIELD_FLAG(ACT_PUSHING_DOOR);
    BIND_BITFIELD_FLAG(ACT_WARP_DOOR_SPAWN);
    BIND_BITFIELD_FLAG(ACT_EMERGE_FROM_PIPE);
    BIND_BITFIELD_FLAG(ACT_SPAWN_SPIN_AIRBORNE);
    BIND_BITFIELD_FLAG(ACT_SPAWN_SPIN_LANDING);
    BIND_BITFIELD_FLAG(ACT_EXIT_AIRBORNE);
    BIND_BITFIELD_FLAG(ACT_EXIT_LAND_SAVE_DIALOG);
    BIND_BITFIELD_FLAG(ACT_DEATH_EXIT);
    BIND_BITFIELD_FLAG(ACT_UNUSED_DEATH_EXIT);
    BIND_BITFIELD_FLAG(ACT_FALLING_DEATH_EXIT);
    BIND_BITFIELD_FLAG(ACT_SPECIAL_EXIT_AIRBORNE);
    BIND_BITFIELD_FLAG(ACT_SPECIAL_DEATH_EXIT);
    BIND_BITFIELD_FLAG(ACT_FALLING_EXIT_AIRBORNE);
    BIND_BITFIELD_FLAG(ACT_UNLOCKING_KEY_DOOR);
    BIND_BITFIELD_FLAG(ACT_UNLOCKING_STAR_DOOR);
    BIND_BITFIELD_FLAG(ACT_ENTERING_STAR_DOOR);
    BIND_BITFIELD_FLAG(ACT_SPAWN_NO_SPIN_AIRBORNE);
    BIND_BITFIELD_FLAG(ACT_SPAWN_NO_SPIN_LANDING);
    BIND_BITFIELD_FLAG(ACT_BBH_ENTER_JUMP);
    BIND_BITFIELD_FLAG(ACT_BBH_ENTER_SPIN);
    BIND_BITFIELD_FLAG(ACT_TELEPORT_FADE_OUT);
    BIND_BITFIELD_FLAG(ACT_TELEPORT_FADE_IN);
    BIND_BITFIELD_FLAG(ACT_SHOCKED);
    BIND_BITFIELD_FLAG(ACT_SQUISHED);
    BIND_BITFIELD_FLAG(ACT_HEAD_STUCK_IN_GROUND);
    BIND_BITFIELD_FLAG(ACT_BUTT_STUCK_IN_GROUND);
    BIND_BITFIELD_FLAG(ACT_FEET_STUCK_IN_GROUND);
    BIND_BITFIELD_FLAG(ACT_PUTTING_ON_CAP);
    BIND_BITFIELD_FLAG(ACT_HOLDING_POLE);
    BIND_BITFIELD_FLAG(ACT_GRAB_POLE_SLOW);
    BIND_BITFIELD_FLAG(ACT_GRAB_POLE_FAST);
    BIND_BITFIELD_FLAG(ACT_CLIMBING_POLE);
    BIND_BITFIELD_FLAG(ACT_TOP_OF_POLE_TRANSITION);
    BIND_BITFIELD_FLAG(ACT_TOP_OF_POLE);
    BIND_BITFIELD_FLAG(ACT_START_HANGING);
    BIND_BITFIELD_FLAG(ACT_HANGING);
    BIND_BITFIELD_FLAG(ACT_HANG_MOVING);
    BIND_BITFIELD_FLAG(ACT_LEDGE_GRAB);
    BIND_BITFIELD_FLAG(ACT_LEDGE_CLIMB_SLOW_1);
    BIND_BITFIELD_FLAG(ACT_LEDGE_CLIMB_SLOW_2);
    BIND_BITFIELD_FLAG(ACT_LEDGE_CLIMB_DOWN);
    BIND_BITFIELD_FLAG(ACT_LEDGE_CLIMB_FAST);
    BIND_BITFIELD_FLAG(ACT_GRABBED);
    BIND_BITFIELD_FLAG(ACT_IN_CANNON);
    BIND_BITFIELD_FLAG(ACT_TORNADO_TWIRLING);
    BIND_BITFIELD_FLAG(ACT_PUNCHING);
    BIND_BITFIELD_FLAG(ACT_PICKING_UP);
    BIND_BITFIELD_FLAG(ACT_DIVE_PICKING_UP);
    BIND_BITFIELD_FLAG(ACT_STOMACH_SLIDE_STOP);
    BIND_BITFIELD_FLAG(ACT_PLACING_DOWN);
    BIND_BITFIELD_FLAG(ACT_THROWING);
    BIND_BITFIELD_FLAG(ACT_HEAVY_THROW);
    BIND_BITFIELD_FLAG(ACT_PICKING_UP_BOWSER);
    BIND_BITFIELD_FLAG(ACT_HOLDING_BOWSER);
    BIND_BITFIELD_FLAG(ACT_RELEASING_BOWSER);

    BIND_ENUM_CONSTANT(SEQ_PLAYER_LEVEL);
    BIND_ENUM_CONSTANT(SEQ_PLAYER_ENV);
    BIND_ENUM_CONSTANT(SEQ_PLAYER_SFX);

    BIND_CONSTANT(SEQ_VARIATION);

    BIND_ENUM_CONSTANT(SEQ_SOUND_PLAYER);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_COLLECT_STAR);
    BIND_ENUM_CONSTANT(SEQ_MENU_TITLE_SCREEN);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_GRASS);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_INSIDE_CASTLE);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_WATER);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_HOT);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_BOSS_KOOPA);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_SNOW);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_SLIDE);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_SPOOKY);
    BIND_ENUM_CONSTANT(SEQ_EVENT_PIRANHA_PLANT);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_UNDERGROUND);
    BIND_ENUM_CONSTANT(SEQ_MENU_STAR_SELECT);
    BIND_ENUM_CONSTANT(SEQ_EVENT_POWERUP);
    BIND_ENUM_CONSTANT(SEQ_EVENT_METAL_CAP);
    BIND_ENUM_CONSTANT(SEQ_EVENT_KOOPA_MESSAGE);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_KOOPA_ROAD);
    BIND_ENUM_CONSTANT(SEQ_EVENT_HIGH_SCORE);
    BIND_ENUM_CONSTANT(SEQ_EVENT_MERRY_GO_ROUND);
    BIND_ENUM_CONSTANT(SEQ_EVENT_RACE);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_STAR_SPAWN);
    BIND_ENUM_CONSTANT(SEQ_EVENT_BOSS);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_COLLECT_KEY);
    BIND_ENUM_CONSTANT(SEQ_EVENT_ENDLESS_STAIRS);
    BIND_ENUM_CONSTANT(SEQ_LEVEL_BOSS_KOOPA_FINAL);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_CREDITS);
    BIND_ENUM_CONSTANT(SEQ_EVENT_SOLVE_PUZZLE);
    BIND_ENUM_CONSTANT(SEQ_EVENT_TOAD_MESSAGE);
    BIND_ENUM_CONSTANT(SEQ_EVENT_PEACH_MESSAGE);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_INTRO);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_VICTORY);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_ENDING);
    BIND_ENUM_CONSTANT(SEQ_MENU_FILE_SELECT);
    BIND_ENUM_CONSTANT(SEQ_EVENT_CUTSCENE_LAKITU);
    BIND_ENUM_CONSTANT(SEQ_COUNT);

    BIND_CONSTANT(SOUNDARGS_SHIFT_SOUNDID);

    BIND_ENUM_CONSTANT(SOUND_TERRAIN_DEFAULT);
    BIND_ENUM_CONSTANT(SOUND_TERRAIN_GRASS);
    BIND_ENUM_CONSTANT(SOUND_TERRAIN_WATER);
    BIND_ENUM_CONSTANT(SOUND_TERRAIN_STONE);
    BIND_ENUM_CONSTANT(SOUND_TERRAIN_SPOOKY);
    BIND_ENUM_CONSTANT(SOUND_TERRAIN_SNOW);
    BIND_ENUM_CONSTANT(SOUND_TERRAIN_ICE);
    BIND_ENUM_CONSTANT(SOUND_TERRAIN_SAND);

    BIND_BITFIELD_FLAG(SOUND_ACTION_TERRAIN_JUMP);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TERRAIN_LANDING);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TERRAIN_STEP);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TERRAIN_STEP_TIPTOE);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TERRAIN_STUCK_IN_GROUND);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TERRAIN_HEAVY_LANDING);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_JUMP);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_LANDING);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_STEP);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_HEAVY_LANDING);
    BIND_BITFIELD_FLAG(SOUND_ACTION_CLAP_HANDS_COLD);
    BIND_BITFIELD_FLAG(SOUND_ACTION_HANGING_STEP);
    BIND_BITFIELD_FLAG(SOUND_ACTION_QUICKSAND_STEP);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_STEP_TIPTOE);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN430);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN431);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN432);
    BIND_BITFIELD_FLAG(SOUND_ACTION_SWIM);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN434);
    BIND_BITFIELD_FLAG(SOUND_ACTION_THROW);
    BIND_BITFIELD_FLAG(SOUND_ACTION_KEY_SWISH);
    BIND_BITFIELD_FLAG(SOUND_ACTION_SPIN);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TWIRL);
    BIND_BITFIELD_FLAG(SOUND_ACTION_CLIMB_UP_TREE);
    BIND_BITFIELD_FLAG(SOUND_ACTION_CLIMB_DOWN_TREE);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNK3C);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN43D);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN43E);
    BIND_BITFIELD_FLAG(SOUND_ACTION_PAT_BACK);
    BIND_BITFIELD_FLAG(SOUND_ACTION_BRUSH_HAIR);
    BIND_BITFIELD_FLAG(SOUND_ACTION_CLIMB_UP_POLE);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_BONK);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNSTUCK_FROM_GROUND);
    BIND_BITFIELD_FLAG(SOUND_ACTION_HIT);
    BIND_BITFIELD_FLAG(SOUND_ACTION_HIT_2);
    BIND_BITFIELD_FLAG(SOUND_ACTION_HIT_3);
    BIND_BITFIELD_FLAG(SOUND_ACTION_BONK);
    BIND_BITFIELD_FLAG(SOUND_ACTION_SHRINK_INTO_BBH);
    BIND_BITFIELD_FLAG(SOUND_ACTION_SWIM_FAST);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_JUMP_WATER);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_LAND_WATER);
    BIND_BITFIELD_FLAG(SOUND_ACTION_METAL_STEP_WATER);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNK53);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNK54);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNK55);
    BIND_BITFIELD_FLAG(SOUND_ACTION_FLYING_FAST);
    BIND_BITFIELD_FLAG(SOUND_ACTION_TELEPORT);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN458);
    BIND_BITFIELD_FLAG(SOUND_ACTION_BOUNCE_OFF_OBJECT);
    BIND_BITFIELD_FLAG(SOUND_ACTION_SIDE_FLIP_UNK);
    BIND_BITFIELD_FLAG(SOUND_ACTION_READ_SIGN);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNKNOWN45C);
    BIND_BITFIELD_FLAG(SOUND_ACTION_UNK5D);
    BIND_BITFIELD_FLAG(SOUND_ACTION_INTRO_UNK45E);
    BIND_BITFIELD_FLAG(SOUND_ACTION_INTRO_UNK45F);
    BIND_BITFIELD_FLAG(SOUND_MOVING_TERRAIN_SLIDE);
    BIND_BITFIELD_FLAG(SOUND_MOVING_TERRAIN_RIDING_SHELL);
    BIND_BITFIELD_FLAG(SOUND_MOVING_LAVA_BURN);
    BIND_BITFIELD_FLAG(SOUND_MOVING_SLIDE_DOWN_POLE);
    BIND_BITFIELD_FLAG(SOUND_MOVING_SLIDE_DOWN_TREE);
    BIND_BITFIELD_FLAG(SOUND_MOVING_QUICKSAND_DEATH);
    BIND_BITFIELD_FLAG(SOUND_MOVING_SHOCKED);
    BIND_BITFIELD_FLAG(SOUND_MOVING_FLYING);
    BIND_BITFIELD_FLAG(SOUND_MOVING_ALMOST_DROWNING);
    BIND_BITFIELD_FLAG(SOUND_MOVING_AIM_CANNON);
    BIND_BITFIELD_FLAG(SOUND_MOVING_UNK1A);
    BIND_BITFIELD_FLAG(SOUND_MOVING_RIDING_SHELL_LAVA);
    BIND_BITFIELD_FLAG(SOUND_MARIO_YAH_WAH_HOO);
    BIND_BITFIELD_FLAG(SOUND_MARIO_HOOHOO);
    BIND_BITFIELD_FLAG(SOUND_MARIO_YAHOO);
    BIND_BITFIELD_FLAG(SOUND_MARIO_UH);
    BIND_BITFIELD_FLAG(SOUND_MARIO_HRMM);
    BIND_BITFIELD_FLAG(SOUND_MARIO_WAH2);
    BIND_BITFIELD_FLAG(SOUND_MARIO_WHOA);
    BIND_BITFIELD_FLAG(SOUND_MARIO_EEUH);
    BIND_BITFIELD_FLAG(SOUND_MARIO_ATTACKED);
    BIND_BITFIELD_FLAG(SOUND_MARIO_OOOF);
    BIND_BITFIELD_FLAG(SOUND_MARIO_OOOF2);
    BIND_BITFIELD_FLAG(SOUND_MARIO_HERE_WE_GO);
    BIND_BITFIELD_FLAG(SOUND_MARIO_YAWNING);
    BIND_BITFIELD_FLAG(SOUND_MARIO_SNORING1);
    BIND_BITFIELD_FLAG(SOUND_MARIO_SNORING2);
    BIND_BITFIELD_FLAG(SOUND_MARIO_WAAAOOOW);
    BIND_BITFIELD_FLAG(SOUND_MARIO_HAHA);
    BIND_BITFIELD_FLAG(SOUND_MARIO_HAHA_2);
    BIND_BITFIELD_FLAG(SOUND_MARIO_UH2);
    BIND_BITFIELD_FLAG(SOUND_MARIO_UH2_2);
    BIND_BITFIELD_FLAG(SOUND_MARIO_ON_FIRE);
    BIND_BITFIELD_FLAG(SOUND_MARIO_DYING);
    BIND_BITFIELD_FLAG(SOUND_MARIO_PANTING_COLD);
    BIND_BITFIELD_FLAG(SOUND_MARIO_PANTING);
    BIND_BITFIELD_FLAG(SOUND_MARIO_COUGHING1);
    BIND_BITFIELD_FLAG(SOUND_MARIO_COUGHING2);
    BIND_BITFIELD_FLAG(SOUND_MARIO_COUGHING3);
    BIND_BITFIELD_FLAG(SOUND_MARIO_PUNCH_YAH);
    BIND_BITFIELD_FLAG(SOUND_MARIO_PUNCH_HOO);
    BIND_BITFIELD_FLAG(SOUND_MARIO_MAMA_MIA);
    BIND_BITFIELD_FLAG(SOUND_MARIO_OKEY_DOKEY);
    BIND_BITFIELD_FLAG(SOUND_MARIO_GROUND_POUND_WAH);
    BIND_BITFIELD_FLAG(SOUND_MARIO_DROWNING);
    BIND_BITFIELD_FLAG(SOUND_MARIO_PUNCH_WAH);
    BIND_BITFIELD_FLAG(SOUND_PEACH_DEAR_MARIO);
    BIND_BITFIELD_FLAG(SOUND_MARIO_YAHOO_WAHA_YIPPEE);
    BIND_BITFIELD_FLAG(SOUND_MARIO_DOH);
    BIND_BITFIELD_FLAG(SOUND_MARIO_GAME_OVER);
    BIND_BITFIELD_FLAG(SOUND_MARIO_HELLO);
    BIND_BITFIELD_FLAG(SOUND_MARIO_PRESS_START_TO_PLAY);
    BIND_BITFIELD_FLAG(SOUND_MARIO_TWIRL_BOUNCE);
    BIND_BITFIELD_FLAG(SOUND_MARIO_SNORING3);
    BIND_BITFIELD_FLAG(SOUND_MARIO_SO_LONGA_BOWSER);
    BIND_BITFIELD_FLAG(SOUND_MARIO_IMA_TIRED);
    BIND_BITFIELD_FLAG(SOUND_PEACH_MARIO);
    BIND_BITFIELD_FLAG(SOUND_PEACH_POWER_OF_THE_STARS);
    BIND_BITFIELD_FLAG(SOUND_PEACH_THANKS_TO_YOU);
    BIND_BITFIELD_FLAG(SOUND_PEACH_THANK_YOU_MARIO);
    BIND_BITFIELD_FLAG(SOUND_PEACH_SOMETHING_SPECIAL);
    BIND_BITFIELD_FLAG(SOUND_PEACH_BAKE_A_CAKE);
    BIND_BITFIELD_FLAG(SOUND_PEACH_FOR_MARIO);
    BIND_BITFIELD_FLAG(SOUND_PEACH_MARIO2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_ACTIVATE_CAP_SWITCH);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_FLAME_OUT);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_OPEN_WOOD_DOOR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CLOSE_WOOD_DOOR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_OPEN_IRON_DOOR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CLOSE_IRON_DOOR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BUBBLES);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_MOVING_WATER);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SWISH_WATER);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_QUIET_BUBBLE);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_VOLCANO_EXPLOSION);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_QUIET_BUBBLE2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CASTLE_TRAP_OPEN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_WALL_EXPLOSION);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_COIN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_COIN_WATER);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHORT_STAR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BIG_CLOCK);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_LOUD_POUND);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_LOUD_POUND2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHORT_POUND1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHORT_POUND2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHORT_POUND3);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHORT_POUND4);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHORT_POUND5);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHORT_POUND6);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_OPEN_CHEST);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CLAM_SHELL1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOX_LANDING);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOX_LANDING_2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNKNOWN1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNKNOWN1_2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CLAM_SHELL2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CLAM_SHELL3);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_PAINTING_EJECT);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_LEVEL_SELECT_CHANGE);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_PLATFORM);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_DONUT_PLATFORM_EXPLOSION);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOWSER_BOMB_EXPLOSION);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_COIN_SPURT);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_EXPLOSION6);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNK32);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOAT_TILT1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOAT_TILT2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_COIN_DROP);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNKNOWN3);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_PENDULUM_SWING);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CHAIN_CHOMP1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CHAIN_CHOMP2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_DOOR_TURN_KEY);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_MOVING_IN_SAND);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNKNOWN4_LOWPRIO);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNKNOWN4);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_MOVING_PLATFORM_SWITCH);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CAGE_OPEN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_QUIET_POUND1_LOWPRIO);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_QUIET_POUND1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BREAK_BOX);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_DOOR_INSERT_KEY);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_QUIET_POUND2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BIG_POUND);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNK45);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_UNK46);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_CANNON_UP);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_GRINDEL_ROLL);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_EXPLOSION7);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SHAKE_COFFIN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_RACE_GUN_SHOT);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_STAR_DOOR_OPEN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_STAR_DOOR_CLOSE);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_POUND_ROCK);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_STAR_APPEARS);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_COLLECT_1UP);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BUTTON_PRESS_LOWPRIO);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BUTTON_PRESS);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BUTTON_PRESS_2_LOWPRIO);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BUTTON_PRESS_2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_ELEVATOR_MOVE);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_ELEVATOR_MOVE_2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SWISH_AIR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SWISH_AIR_2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_HAUNTED_CHAIR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SOFT_LANDING);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_HAUNTED_CHAIR_MOVE);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOWSER_PLATFORM);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOWSER_PLATFORM_2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_HEART_SPIN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_POUND_WOOD_POST);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_WATER_LEVEL_TRIG);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SWITCH_DOOR_OPEN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_RED_COIN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BIRDS_FLY_AWAY);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_METAL_POUND);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOING1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOING2_LOWPRIO);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOING2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_YOSHI_WALK);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_ENEMY_ALERT1);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_YOSHI_TALK);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_SPLATTERING);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOING3);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_GRAND_STAR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_GRAND_STAR_JUMP);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_BOAT_ROCK);
    BIND_BITFIELD_FLAG(SOUND_GENERAL_VANISH_SFX);
    BIND_BITFIELD_FLAG(SOUND_ENV_WATERFALL1);
    BIND_BITFIELD_FLAG(SOUND_ENV_WATERFALL2);
    BIND_BITFIELD_FLAG(SOUND_ENV_ELEVATOR1);
    BIND_BITFIELD_FLAG(SOUND_ENV_DRONING1);
    BIND_BITFIELD_FLAG(SOUND_ENV_DRONING2);
    BIND_BITFIELD_FLAG(SOUND_ENV_WIND1);
    BIND_BITFIELD_FLAG(SOUND_ENV_MOVING_SAND_SNOW);
    BIND_BITFIELD_FLAG(SOUND_ENV_UNK07);
    BIND_BITFIELD_FLAG(SOUND_ENV_ELEVATOR2);
    BIND_BITFIELD_FLAG(SOUND_ENV_WATER);
    BIND_BITFIELD_FLAG(SOUND_ENV_UNKNOWN2);
    BIND_BITFIELD_FLAG(SOUND_ENV_BOAT_ROCKING1);
    BIND_BITFIELD_FLAG(SOUND_ENV_ELEVATOR3);
    BIND_BITFIELD_FLAG(SOUND_ENV_ELEVATOR4);
    BIND_BITFIELD_FLAG(SOUND_ENV_ELEVATOR4_2);
    BIND_BITFIELD_FLAG(SOUND_ENV_MOVINGSAND);
    BIND_BITFIELD_FLAG(SOUND_ENV_MERRY_GO_ROUND_CREAKING);
    BIND_BITFIELD_FLAG(SOUND_ENV_WIND2);
    BIND_BITFIELD_FLAG(SOUND_ENV_UNK12);
    BIND_BITFIELD_FLAG(SOUND_ENV_SLIDING);
    BIND_BITFIELD_FLAG(SOUND_ENV_STAR);
    BIND_BITFIELD_FLAG(SOUND_ENV_MOVING_BIG_PLATFORM);
    BIND_BITFIELD_FLAG(SOUND_ENV_WATER_DRAIN);
    BIND_BITFIELD_FLAG(SOUND_ENV_METAL_BOX_PUSH);
    BIND_BITFIELD_FLAG(SOUND_ENV_SINK_QUICKSAND);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SUSHI_SHARK_WATER_SOUND);
    BIND_BITFIELD_FLAG(SOUND_OBJ_MRI_SHOOT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BABY_PENGUIN_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOWSER_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOWSER_TAIL_PICKUP);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOWSER_DEFEATED);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOWSER_SPINNING);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOWSER_INHALING);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BIG_PENGUIN_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOO_BOUNCE_TOP);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOO_LAUGH_SHORT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_THWOMP);
    BIND_BITFIELD_FLAG(SOUND_OBJ_CANNON1);
    BIND_BITFIELD_FLAG(SOUND_OBJ_CANNON2);
    BIND_BITFIELD_FLAG(SOUND_OBJ_CANNON3);
    BIND_BITFIELD_FLAG(SOUND_OBJ_JUMP_WALK_WATER);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UNKNOWN2);
    BIND_BITFIELD_FLAG(SOUND_OBJ_MRI_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_POUNDING1);
    BIND_BITFIELD_FLAG(SOUND_OBJ_POUNDING1_HIGHPRIO);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WHOMP);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KING_BOBOMB);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BULLY_METAL);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BULLY_EXPLODE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BULLY_EXPLODE_2);
    BIND_BITFIELD_FLAG(SOUND_OBJ_POUNDING_CANNON);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BULLY_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UNKNOWN3);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UNKNOWN4);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BABY_PENGUIN_DIVE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_GOOMBA_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UKIKI_CHATTER_LONG);
    BIND_BITFIELD_FLAG(SOUND_OBJ_MONTY_MOLE_ATTACK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_EVIL_LAKITU_THROW);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UNK23);
    BIND_BITFIELD_FLAG(SOUND_OBJ_DYING_ENEMY1);
    BIND_BITFIELD_FLAG(SOUND_OBJ_CANNON4);
    BIND_BITFIELD_FLAG(SOUND_OBJ_DYING_ENEMY2);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOBOMB_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SOMETHING_LANDING);
    BIND_BITFIELD_FLAG(SOUND_OBJ_DIVING_IN_WATER);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SNOW_SAND1);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SNOW_SAND2);
    BIND_BITFIELD_FLAG(SOUND_OBJ_DEFAULT_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BIG_PENGUIN_YELL);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WATER_BOMB_BOUNCING);
    BIND_BITFIELD_FLAG(SOUND_OBJ_GOOMBA_ALERT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WIGGLER_JUMP);
    BIND_BITFIELD_FLAG(SOUND_OBJ_STOMPED);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UNKNOWN6);
    BIND_BITFIELD_FLAG(SOUND_OBJ_DIVING_INTO_WATER);
    BIND_BITFIELD_FLAG(SOUND_OBJ_PIRANHA_PLANT_SHRINK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KOOPA_THE_QUICK_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KOOPA_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BULLY_WALKING);
    BIND_BITFIELD_FLAG(SOUND_OBJ_DORRIE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOWSER_LAUGH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UKIKI_CHATTER_SHORT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UKIKI_CHATTER_IDLE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UKIKI_STEP_DEFAULT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_UKIKI_STEP_LEAVES);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KOOPA_TALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KOOPA_DAMAGE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KLEPTO1);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KLEPTO2);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KING_BOBOMB_TALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KING_BOBOMB_JUMP);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KING_WHOMP_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOO_LAUGH_LONG);
    BIND_BITFIELD_FLAG(SOUND_OBJ_EEL);
    BIND_BITFIELD_FLAG(SOUND_OBJ_EEL_2);
    BIND_BITFIELD_FLAG(SOUND_OBJ_EYEROK_SHOW_EYE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_MR_BLIZZARD_ALERT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SNUFIT_SHOOT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SKEETER_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WALKING_WATER);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BIRD_CHIRP3);
    BIND_BITFIELD_FLAG(SOUND_OBJ_PIRANHA_PLANT_APPEAR);
    BIND_BITFIELD_FLAG(SOUND_OBJ_FLAME_BLOWN);
    BIND_BITFIELD_FLAG(SOUND_OBJ_MAD_PIANO_CHOMPING);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOBOMB_BUDDY_TALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SPINY_UNK59);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WIGGLER_HIGH_PITCH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_HEAVEHO_TOSSED);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WIGGLER_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BOWSER_INTRO_LAUGH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_ENEMY_DEATH_HIGH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_ENEMY_DEATH_LOW);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SWOOP_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_KOOPA_FLYGUY_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_POKEY_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SNOWMAN_BOUNCE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SNOWMAN_EXPLODE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_POUNDING_LOUD);
    BIND_BITFIELD_FLAG(SOUND_OBJ_MIPS_RABBIT);
    BIND_BITFIELD_FLAG(SOUND_OBJ_MIPS_RABBIT_WATER);
    BIND_BITFIELD_FLAG(SOUND_OBJ_EYEROK_EXPLODE);
    BIND_BITFIELD_FLAG(SOUND_OBJ_CHUCKYA_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WIGGLER_TALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WIGGLER_ATTACKED);
    BIND_BITFIELD_FLAG(SOUND_OBJ_WIGGLER_LOW_PITCH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_SNUFIT_SKEETER_DEATH);
    BIND_BITFIELD_FLAG(SOUND_OBJ_BUBBA_CHOMP);
    BIND_BITFIELD_FLAG(SOUND_OBJ_ENEMY_DEFEAT_SHRINK);
    BIND_BITFIELD_FLAG(SOUND_AIR_BOWSER_SPIT_FIRE);
    BIND_BITFIELD_FLAG(SOUND_AIR_UNK01);
    BIND_BITFIELD_FLAG(SOUND_AIR_LAKITU_FLY);
    BIND_BITFIELD_FLAG(SOUND_AIR_LAKITU_FLY_HIGHPRIO);
    BIND_BITFIELD_FLAG(SOUND_AIR_AMP_BUZZ);
    BIND_BITFIELD_FLAG(SOUND_AIR_BLOW_FIRE);
    BIND_BITFIELD_FLAG(SOUND_AIR_BLOW_WIND);
    BIND_BITFIELD_FLAG(SOUND_AIR_ROUGH_SLIDE);
    BIND_BITFIELD_FLAG(SOUND_AIR_HEAVEHO_MOVE);
    BIND_BITFIELD_FLAG(SOUND_AIR_UNK07);
    BIND_BITFIELD_FLAG(SOUND_AIR_BOBOMB_LIT_FUSE);
    BIND_BITFIELD_FLAG(SOUND_AIR_HOWLING_WIND);
    BIND_BITFIELD_FLAG(SOUND_AIR_CHUCKYA_MOVE);
    BIND_BITFIELD_FLAG(SOUND_AIR_PEACH_TWINKLE);
    BIND_BITFIELD_FLAG(SOUND_AIR_CASTLE_OUTDOORS_AMBIENT);
    BIND_BITFIELD_FLAG(SOUND_MENU_CHANGE_SELECT);
    BIND_BITFIELD_FLAG(SOUND_MENU_REVERSE_PAUSE);
    BIND_BITFIELD_FLAG(SOUND_MENU_PAUSE);
    BIND_BITFIELD_FLAG(SOUND_MENU_PAUSE_2);
    BIND_BITFIELD_FLAG(SOUND_MENU_MESSAGE_APPEAR);
    BIND_BITFIELD_FLAG(SOUND_MENU_MESSAGE_DISAPPEAR);
    BIND_BITFIELD_FLAG(SOUND_MENU_CAMERA_ZOOM_IN);
    BIND_BITFIELD_FLAG(SOUND_MENU_CAMERA_ZOOM_OUT);
    BIND_BITFIELD_FLAG(SOUND_MENU_PINCH_MARIO_FACE);
    BIND_BITFIELD_FLAG(SOUND_MENU_LET_GO_MARIO_FACE);
    BIND_BITFIELD_FLAG(SOUND_MENU_HAND_APPEAR);
    BIND_BITFIELD_FLAG(SOUND_MENU_HAND_DISAPPEAR);
    BIND_BITFIELD_FLAG(SOUND_MENU_UNK0C);
    BIND_BITFIELD_FLAG(SOUND_MENU_POWER_METER);
    BIND_BITFIELD_FLAG(SOUND_MENU_CAMERA_BUZZ);
    BIND_BITFIELD_FLAG(SOUND_MENU_CAMERA_TURN);
    BIND_BITFIELD_FLAG(SOUND_MENU_UNK10);
    BIND_BITFIELD_FLAG(SOUND_MENU_CLICK_FILE_SELECT);
    BIND_BITFIELD_FLAG(SOUND_MENU_MESSAGE_NEXT_PAGE);
    BIND_BITFIELD_FLAG(SOUND_MENU_COIN_ITS_A_ME_MARIO);
    BIND_BITFIELD_FLAG(SOUND_MENU_YOSHI_GAIN_LIVES);
    BIND_BITFIELD_FLAG(SOUND_MENU_ENTER_PIPE);
    BIND_BITFIELD_FLAG(SOUND_MENU_EXIT_PIPE);
    BIND_BITFIELD_FLAG(SOUND_MENU_BOWSER_LAUGH);
    BIND_BITFIELD_FLAG(SOUND_MENU_ENTER_HOLE);
    BIND_BITFIELD_FLAG(SOUND_MENU_CLICK_CHANGE_VIEW);
    BIND_BITFIELD_FLAG(SOUND_MENU_CAMERA_UNUSED1);
    BIND_BITFIELD_FLAG(SOUND_MENU_CAMERA_UNUSED2);
    BIND_BITFIELD_FLAG(SOUND_MENU_MARIO_CASTLE_WARP);
    BIND_BITFIELD_FLAG(SOUND_MENU_STAR_SOUND);
    BIND_BITFIELD_FLAG(SOUND_MENU_THANK_YOU_PLAYING_MY_GAME);
    BIND_BITFIELD_FLAG(SOUND_MENU_READ_A_SIGN);
    BIND_BITFIELD_FLAG(SOUND_MENU_EXIT_A_SIGN);
    BIND_BITFIELD_FLAG(SOUND_MENU_MARIO_CASTLE_WARP2);
    BIND_BITFIELD_FLAG(SOUND_MENU_STAR_SOUND_OKEY_DOKEY);
    BIND_BITFIELD_FLAG(SOUND_MENU_STAR_SOUND_LETS_A_GO);
    BIND_BITFIELD_FLAG(SOUND_MENU_COLLECT_RED_COIN);
    BIND_BITFIELD_FLAG(SOUND_MENU_COLLECT_SECRET);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_BOBOMB_EXPLOSION);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_PURPLE_SWITCH);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_ROTATING_BLOCK_CLICK);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_SPINDEL_ROLL);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_PYRAMID_TOP_SPIN);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_PYRAMID_TOP_EXPLOSION);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_BIRD_CHIRP2);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_SWITCH_TICK_FAST);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_SWITCH_TICK_SLOW);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_STAR_APPEARS);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_ROTATING_BLOCK_ALERT);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_BOWSER_EXPLODE);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_BOWSER_KEY);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_1UP_APPEAR);
    BIND_BITFIELD_FLAG(SOUND_GENERAL2_RIGHT_ANSWER);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_BOWSER_ROAR);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_PIRANHA_PLANT_BITE);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_PIRANHA_PLANT_DYING);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_BOWSER_PUZZLE_PIECE_MOVE);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_BULLY_ATTACKED);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_KING_BOBOMB_DAMAGE);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_SCUTTLEBUG_WALK);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_SCUTTLEBUG_ALERT);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_BABY_PENGUIN_YELL);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_SWOOP);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_BIRD_CHIRP1);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_LARGE_BULLY_ATTACKED);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_EYEROK_SOUND_SHORT);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_WHOMP_SOUND_SHORT);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_EYEROK_SOUND_LONG);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_BOWSER_TELEPORT);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_MONTY_MOLE_APPEAR);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_BOSS_DIALOG_GRUNT);
    BIND_BITFIELD_FLAG(SOUND_OBJ2_MRI_SPINNING);
}
