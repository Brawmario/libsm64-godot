#include <sm64_mario_internal.hpp>

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#include <sm64_global.hpp>

#define LERP(last, current, amount) (last + (current - last) * amount)

#define CONVERT_RADIANS_TO_SM64(x) (-(x) / Math_PI * 32768.0)

constexpr real_t g_sm64_fps = 1.0/30.0;

static void invert_vertex_order_2d(float *p_arr, size_t p_triangle_count)
{
    const size_t arr_size = p_triangle_count * 6;
    for (size_t i = 0; i < arr_size; i += 6)
    {
        float temp_x = p_arr[i+0];
        float temp_y = p_arr[i+1];
        p_arr[i+0] = p_arr[i+2];
        p_arr[i+1] = p_arr[i+3];
        p_arr[i+2] = temp_x;
        p_arr[i+3] = temp_y;
    }
}

static void invert_vertex_order_3d(float *p_arr, size_t p_triangle_count)
{
    const size_t arr_size = p_triangle_count * 9;
    for (size_t i = 0; i < arr_size; i += 9)
    {
        float temp_x = p_arr[i+0];
        float temp_y = p_arr[i+1];
        float temp_z = p_arr[i+2];
        p_arr[i+0] = p_arr[i+3];
        p_arr[i+1] = p_arr[i+4];
        p_arr[i+2] = p_arr[i+5];
        p_arr[i+3] = temp_x;
        p_arr[i+4] = temp_y;
        p_arr[i+5] = temp_z;
    }
}

static void lerp(struct SM64MarioState &out, const struct SM64MarioState &last, const struct SM64MarioState &current, float amount)
{
    // Copy current first to account for unlerped members
    out = current;

    for (int i = 0; i < 3; i++)
        out.position[i] = LERP(last.position[i], current.position[i], amount);

    for (int i = 0; i < 3; i++)
        out.velocity[i] = LERP(last.velocity[i], current.velocity[i], amount);

    out.faceAngle = LERP(last.faceAngle, current.faceAngle, amount);
}

int SM64MarioInternal::mario_create(godot::Vector3 p_position, godot::Vector3 p_rotation)
{
    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL_V(sm64_global, -1);
    ERR_FAIL_COND_V_MSG(!sm64_global->is_init(), -1, "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();
    float x = (float) ( p_position.z * scale_factor);
    float y = (float) ( p_position.y * scale_factor);
    float z = (float) (-p_position.x * scale_factor);
    // Unused for now
    // int16_t rx = (int16_t) CONVERT_RADIANS_TO_SM64(-p_rotation.z);
    // int16_t ry = (int16_t) CONVERT_RADIANS_TO_SM64(-p_rotation.y);
    // int16_t rz = (int16_t) CONVERT_RADIANS_TO_SM64( p_rotation.x);

    m_id = sm64_mario_create(x, y, z);

    return m_id;
}

godot::Dictionary SM64MarioInternal::tick(real_t delta, godot::Dictionary p_input)
{
    godot::Dictionary ret;
    godot::Array mesh_array;

    ERR_FAIL_COND_V_MSG(m_id < 0, ret, "[libsm64-godot] tried to tick non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL_V(sm64_global, ret);
    ERR_FAIL_COND_V_MSG(!sm64_global->is_init(), ret, "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    const godot::Vector2 cam_look = p_input["cam_look"];
    const godot::Vector2 stick = p_input["stick"];
    const bool a = p_input["a"];
    const bool b = p_input["b"];
    const bool z = p_input["z"];

    const struct SM64MarioInputs inputs = {
        -cam_look.y, // camLookX
        cam_look.x, // camLookZ
        stick.x,
        stick.y,
        (uint8_t) a,
        (uint8_t) b,
        (uint8_t) z
    };

    // First call will always trigger a tick
    if (unlikely(m_first_tick))
    {
        sm64_mario_tick(m_id, &inputs, &m_out_state_hard_tick[m_current_index], m_geometry_hard_tick[m_current_index].c_handle());
        m_out_state_hard_tick[m_last_index] = m_out_state_hard_tick[m_current_index];
        m_geometry_hard_tick[m_last_index] = m_geometry_hard_tick[m_current_index];

        m_first_tick = false;
    }
    else
    {
        m_time_since_last_tick += delta;
        while (m_time_since_last_tick >= g_sm64_fps)
        {
            // Old current becomes new last, old last will be overwritten by new current
            SWAP(m_current_index, m_last_index);

            sm64_mario_tick(m_id, &inputs, &m_out_state_hard_tick[m_current_index], m_geometry_hard_tick[m_current_index].c_handle());

            m_time_since_last_tick -= g_sm64_fps;
        }
    }

    // Interpolation
    lerp(m_out_state, m_out_state_hard_tick[m_last_index], m_out_state_hard_tick[m_current_index], m_time_since_last_tick / g_sm64_fps);
    const real_t invinc_timer_lerped = godot::MAX(0.0f, (m_out_state.invincTimer * g_sm64_fps) - m_time_since_last_tick);
    m_geometry.lerp(m_geometry_hard_tick[m_last_index], m_geometry_hard_tick[m_current_index], m_time_since_last_tick / g_sm64_fps);

    ret["position"]       = godot::Vector3(-m_out_state.position[2] / scale_factor,
                                            m_out_state.position[1] / scale_factor,
                                            m_out_state.position[0] / scale_factor);
    ret["velocity"]       = godot::Vector3(-m_out_state.velocity[2] / scale_factor,
                                            m_out_state.velocity[1] / scale_factor,
                                            m_out_state.velocity[0] / scale_factor);
    ret["face_angle"]     = (real_t) m_out_state.faceAngle;
    ret["health"]         = (int)    m_out_state.health;
    ret["action"]         = (int)    m_out_state.action;
    ret["flags"]          = (int)    m_out_state.flags;
    ret["particle_flags"] = (int)    m_out_state.particleFlags;
    ret["invinc_timer"]   = (real_t) invinc_timer_lerped;
    // ret["hurt_counter"]   = (int)    m_out_state.hurtCounter;
    // ret["num_lives"]      = (int)    m_out_state.numLives;
    // ret["holding_object"] = (bool)   m_out_state.holdingObject;
    // ret["drop_method"]    = (int)    m_out_state.dropMethod;

    const int vertex_count = m_geometry.triangles_used() * 3;

    mesh_array.resize(godot::ArrayMesh::ARRAY_MAX);
    m_position.resize(vertex_count);
    m_normal.resize(vertex_count);
    m_color.resize(vertex_count);
    m_uv.resize(vertex_count);

    // SM64 to godot conversion
    invert_vertex_order_3d(m_geometry.position, m_geometry.triangles_used());
    invert_vertex_order_3d(m_geometry.normal,   m_geometry.triangles_used());
    invert_vertex_order_3d(m_geometry.color,    m_geometry.triangles_used());
    invert_vertex_order_2d(m_geometry.uv,       m_geometry.triangles_used());

    godot::Vector3 *position_ptrw = m_position.ptrw();
    for (int i = 0; i < vertex_count; i += 3)
    {
        position_ptrw[i+0].z =  m_geometry.position[3*i+0] / scale_factor;
        position_ptrw[i+0].y =  m_geometry.position[3*i+1] / scale_factor;
        position_ptrw[i+0].x = -m_geometry.position[3*i+2] / scale_factor;
        position_ptrw[i+1].z =  m_geometry.position[3*i+3] / scale_factor;
        position_ptrw[i+1].y =  m_geometry.position[3*i+4] / scale_factor;
        position_ptrw[i+1].x = -m_geometry.position[3*i+5] / scale_factor;
        position_ptrw[i+2].z =  m_geometry.position[3*i+6] / scale_factor;
        position_ptrw[i+2].y =  m_geometry.position[3*i+7] / scale_factor;
        position_ptrw[i+2].x = -m_geometry.position[3*i+8] / scale_factor;
    }
    godot::Vector3 *normal_ptrw = m_normal.ptrw();
    for (int i = 0; i < vertex_count; i += 3)
    {
        normal_ptrw[i+0].z =  m_geometry.normal[3*i+0];
        normal_ptrw[i+0].y =  m_geometry.normal[3*i+1];
        normal_ptrw[i+0].x = -m_geometry.normal[3*i+2];
        normal_ptrw[i+1].z =  m_geometry.normal[3*i+3];
        normal_ptrw[i+1].y =  m_geometry.normal[3*i+4];
        normal_ptrw[i+1].x = -m_geometry.normal[3*i+5];
        normal_ptrw[i+2].z =  m_geometry.normal[3*i+6];
        normal_ptrw[i+2].y =  m_geometry.normal[3*i+7];
        normal_ptrw[i+2].x = -m_geometry.normal[3*i+8];
    }
    godot::Color *color_ptrw = m_color.ptrw();
    for (int i = 0; i < vertex_count; i += 3)
    {
        color_ptrw[i+0] = godot::Color(m_geometry.color[3*i+0],
                                       m_geometry.color[3*i+1],
                                       m_geometry.color[3*i+2]);
        color_ptrw[i+1] = godot::Color(m_geometry.color[3*i+3],
                                       m_geometry.color[3*i+4],
                                       m_geometry.color[3*i+5]);
        color_ptrw[i+2] = godot::Color(m_geometry.color[3*i+6],
                                       m_geometry.color[3*i+7],
                                       m_geometry.color[3*i+8]);
    }
    godot::Vector2 *uv_ptrw = m_uv.ptrw();
    for (int i = 0; i < vertex_count; i += 3)
    {
        uv_ptrw[i+0] = godot::Vector2(m_geometry.uv[2*i+0],
                                      m_geometry.uv[2*i+1]);
        uv_ptrw[i+1] = godot::Vector2(m_geometry.uv[2*i+2],
                                      m_geometry.uv[2*i+3]);
        uv_ptrw[i+2] = godot::Vector2(m_geometry.uv[2*i+4],
                                      m_geometry.uv[2*i+5]);
    }

    mesh_array[godot::ArrayMesh::ARRAY_VERTEX] = m_position;
    mesh_array[godot::ArrayMesh::ARRAY_NORMAL] = m_normal;
    mesh_array[godot::ArrayMesh::ARRAY_COLOR]  = m_color;
    mesh_array[godot::ArrayMesh::ARRAY_TEX_UV] = m_uv;

    ret["mesh_array"] = mesh_array;

    return ret;
}

void SM64MarioInternal::mario_delete()
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Tried to delete non existent Mario");

    sm64_mario_delete(m_id);
}

void SM64MarioInternal::set_action(int p_action)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_set_mario_action(m_id, (uint32_t) p_action);
}

void SM64MarioInternal::set_state(int p_flags)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_set_mario_state(m_id, (uint32_t) p_flags);
}

void SM64MarioInternal::set_position(godot::Vector3 p_position)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_position(m_id,
                             p_position.z * scale_factor,
                             p_position.y * scale_factor,
                            -p_position.x * scale_factor);
}

void SM64MarioInternal::set_angle(godot::Vector3 p_rotation)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_set_mario_angle(m_id,
                          p_rotation.z,
                          p_rotation.y,
                         -p_rotation.x);
}

void SM64MarioInternal::set_face_angle(real_t p_value)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_set_mario_faceangle(m_id, p_value);
}

void SM64MarioInternal::set_velocity(godot::Vector3 p_velocity)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_velocity(m_id,
                             p_velocity.z * scale_factor,
                             p_velocity.y * scale_factor,
                            -p_velocity.x * scale_factor);
}

void SM64MarioInternal::set_forward_velocity(real_t p_velocity)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_forward_velocity(m_id, p_velocity * scale_factor);
}

// void SM64MarioInternal::set_invincibility(real_t p_time)
// {
//     ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");
//
//     sm64_set_mario_invincibility(m_id, (int16_t) (p_time / g_sm64_fps));
// }

void SM64MarioInternal::set_water_level(real_t p_level)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_water_level(m_id, (int) p_level * scale_factor);
}

// void SM64MarioInternal::set_floor_override(godot::Ref<SM64SurfaceProperties> p_surface_properties)
// {
//     ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");
//     ERR_FAIL_COND_MSG(p_surface_properties.is_null(), "[libsm64-godot] Called set_floor_override with null surface_properties.");
//
//     sm64_set_mario_floor_override(m_id,
//                                   p_surface_properties->get_terrain_type(),
//                                   p_surface_properties->get_surface_type(),
//                                   p_surface_properties->get_force());
// }

// void SM64MarioInternal::reset_floor_override()
// {
//     ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");
//
//     sm64_set_mario_floor_override(m_id, 0x7, 0x100, 0);
// }

// void SM64MarioInternal::set_health(int p_health)
// {
//     ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");
//
//     sm64_set_mario_health(m_id, p_health);
// }

void SM64MarioInternal::take_damage(int p_damage, godot::Vector3 p_source_position, bool p_big_knockback)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_mario_take_damage(m_id,
                            p_damage,
                            p_big_knockback ? 0x8 : 0x0,
                            p_source_position.z * scale_factor,
                            p_source_position.y * scale_factor,
                           -p_source_position.x * scale_factor);
}

void SM64MarioInternal::heal(int p_heal_counter)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_mario_heal(m_id, p_heal_counter);
}

// void SM64MarioInternal::set_lives(int p_lives)
// {
//     ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");
//
//     sm64_mario_set_lives(m_id, p_lives);
// }

void SM64MarioInternal::interact_cap(int p_cap, real_t p_cap_time, bool p_play_music)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_mario_interact_cap(m_id, (uint32_t) p_cap, (uint16_t) (p_cap_time / g_sm64_fps), (uint8_t) p_play_music);
}

// void SM64MarioInternal::extend_cap(real_t p_cap_time)
// {
//     ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");
//
//     sm64_mario_extend_cap(m_id, (uint16_t) (p_cap_time / g_sm64_fps));
// }

void SM64MarioInternal::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("mario_create", "position", "rotation"), &SM64MarioInternal::mario_create);
    godot::ClassDB::bind_method(godot::D_METHOD("tick", "input"), &SM64MarioInternal::tick);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_delete"), &SM64MarioInternal::mario_delete);
    godot::ClassDB::bind_method(godot::D_METHOD("set_action", "action"), &SM64MarioInternal::set_action);
    godot::ClassDB::bind_method(godot::D_METHOD("set_state", "flags"), &SM64MarioInternal::set_state);
    godot::ClassDB::bind_method(godot::D_METHOD("set_position", "position"), &SM64MarioInternal::set_position);
    godot::ClassDB::bind_method(godot::D_METHOD("set_angle", "rotation"), &SM64MarioInternal::set_angle);
    godot::ClassDB::bind_method(godot::D_METHOD("set_face_angle", "value"), &SM64MarioInternal::set_face_angle);
    godot::ClassDB::bind_method(godot::D_METHOD("set_velocity", "velocity"), &SM64MarioInternal::set_velocity);
    godot::ClassDB::bind_method(godot::D_METHOD("set_forward_velocity", "velocity"), &SM64MarioInternal::set_forward_velocity);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_invincibility", "time"), &SM64MarioInternal::set_invincibility);
    godot::ClassDB::bind_method(godot::D_METHOD("set_water_level", "level"), &SM64MarioInternal::set_water_level);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_floor_override", "surface_properties"), &SM64MarioInternal::set_floor_override);
    // godot::ClassDB::bind_method(godot::D_METHOD("reset_floor_override"), &SM64MarioInternal::reset_floor_override);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_health", "health"), &SM64MarioInternal::set_health);
    godot::ClassDB::bind_method(godot::D_METHOD("take_damage", "damage", "source_position", "big_knockback"), &SM64MarioInternal::take_damage, DEFVAL(false));
    godot::ClassDB::bind_method(godot::D_METHOD("heal", "heal_counter"), &SM64MarioInternal::heal);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_lives", "lives"), &SM64MarioInternal::set_lives);
    godot::ClassDB::bind_method(godot::D_METHOD("interact_cap", "cap", "cap_time", "play_music"), &SM64MarioInternal::interact_cap, DEFVAL(0), DEFVAL(true));
    // godot::ClassDB::bind_method(godot::D_METHOD("extend_cap", "cap_time"), &SM64MarioInternal::extend_cap);
}
