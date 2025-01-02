#include <sm64_mario_internal.hpp>

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#include <sm64_global.hpp>

#define CONVERT_RADIANS_TO_SM64(x) (-(x) / Math_PI * 32768.0)

constexpr real_t g_sm64_delta = 1.0/30.0;

template<class It>
_FORCE_INLINE_ static void invert_vertex_order_2d(It p_it, It p_end)
{
    for (; p_it != p_end; p_it += 6)
    {
        std::swap(*(p_it + 0), *(p_it + 2));
        std::swap(*(p_it + 1), *(p_it + 3));
    }
}

template<class It>
_FORCE_INLINE_ static void invert_vertex_order_3d(It p_it, It p_end)
{
    for (; p_it != p_end; p_it += 9)
    {
        std::swap(*(p_it + 0), *(p_it + 3));
        std::swap(*(p_it + 1), *(p_it + 4));
        std::swap(*(p_it + 2), *(p_it + 5));
    }
}

static void lerp(struct SM64MarioState &out, const struct SM64MarioState &last, const struct SM64MarioState &current, float amount)
{
    // Copy current first to account for unlerped members
    out = current;

    for (int i = 0; i < 3; i++)
        out.position[i] = godot::Math::lerp(last.position[i], current.position[i], amount);

    for (int i = 0; i < 3; i++)
        out.velocity[i] = godot::Math::lerp(last.velocity[i], current.velocity[i], amount);

    out.faceAngle = godot::Math::lerp_angle(last.faceAngle, current.faceAngle, amount);
}

void SM64MarioInternal::set_interpolate(bool value)
{
    m_interpolate = value;
    reset_interpolation();
}

bool SM64MarioInternal::get_interpolate() const
{
    return m_interpolate;
}

void SM64MarioInternal::reset_interpolation()
{
    // Making last = current effectively nulls interpolation until next hard tick
    m_out_state_hard_tick[m_last_index] = m_out_state_hard_tick[m_current_index];
    m_geometry_hard_tick[m_last_index] = m_geometry_hard_tick[m_current_index];
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

godot::Dictionary SM64MarioInternal::tick(real_t delta, godot::Ref<SM64Input> p_input)
{
    godot::Dictionary ret;

    ERR_FAIL_NULL_V(p_input, ret);
    ERR_FAIL_COND_V_MSG(m_id < 0, ret, "[libsm64-godot] tried to tick non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL_V(sm64_global, ret);
    ERR_FAIL_COND_V_MSG(!sm64_global->is_init(), ret, "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    const struct SM64MarioInputs inputs = p_input->to_sm64_mario_inputs();

    // First call will always trigger a tick
    if (unlikely(m_first_tick))
    {
        sm64_mario_tick(m_id, &inputs, &m_out_state_hard_tick[m_current_index], m_geometry_hard_tick[m_current_index].c_handle());
        reset_interpolation();

        m_first_tick = false;
    }
    else
    {
        m_time_since_last_tick += delta;
        while (m_time_since_last_tick >= g_sm64_delta)
        {
            // Old current becomes new last, old last will be overwritten by new current
            std::swap(m_current_index, m_last_index);

            sm64_mario_tick(m_id, &inputs, &m_out_state_hard_tick[m_current_index], m_geometry_hard_tick[m_current_index].c_handle());

            if (m_reset_interpolation_next_tick)
            {
                reset_interpolation();
                m_reset_interpolation_next_tick = false;
            }

            m_time_since_last_tick -= g_sm64_delta;
        }
    }

    // Interpolation
    if (m_interpolate)
    {
        lerp(m_out_state, m_out_state_hard_tick[m_last_index], m_out_state_hard_tick[m_current_index], m_time_since_last_tick / g_sm64_delta);
        m_geometry.lerp(m_geometry_hard_tick[m_last_index], m_geometry_hard_tick[m_current_index], m_time_since_last_tick / g_sm64_delta);
    }
    else
    {
        m_out_state = m_out_state_hard_tick[m_current_index];
        m_geometry = m_geometry_hard_tick[m_current_index];
    }

    real_t invinc_timer = m_out_state.invincTimer * g_sm64_delta;
    if (m_interpolate)
    {
        invinc_timer = godot::MAX(0.0f, invinc_timer - m_time_since_last_tick);
    }

    ret["position"]       = godot::Vector3(-m_out_state.position[2] / scale_factor,
                                            m_out_state.position[1] / scale_factor,
                                            m_out_state.position[0] / scale_factor);
    ret["velocity"]       = godot::Vector3(-m_out_state.velocity[2] / (g_sm64_delta * scale_factor),
                                            m_out_state.velocity[1] / (g_sm64_delta * scale_factor),
                                            m_out_state.velocity[0] / (g_sm64_delta * scale_factor));
    ret["face_angle"]     = (real_t) m_out_state.faceAngle;
    ret["health"]         = (int)    m_out_state.health;
    ret["action"]         = (int)    m_out_state.action;
    ret["flags"]          = (int)    m_out_state.flags;
    ret["particle_flags"] = (int)    m_out_state.particleFlags;
    ret["invinc_timer"]   = (real_t) invinc_timer;
    // ret["hurt_counter"]   = (int)    m_out_state.hurtCounter;
    // ret["num_lives"]      = (int)    m_out_state.numLives;
    // ret["holding_object"] = (bool)   m_out_state.holdingObject;
    // ret["drop_method"]    = (int)    m_out_state.dropMethod;

    const int vertex_count = m_geometry.triangles_used() * 3;

    godot::Array mesh_array;
    mesh_array.resize(godot::ArrayMesh::ARRAY_MAX);
    m_position.resize(vertex_count);
    m_normal.resize(vertex_count);
    m_color.resize(vertex_count);
    m_uv.resize(vertex_count);

    // SM64 to godot conversion

    // Winding order: counter-clockwise (SM64) -> clockwise (Godot)
    invert_vertex_order_3d(m_geometry.position.begin(), m_geometry.position.begin() + vertex_count * 3);
    invert_vertex_order_3d(m_geometry.normal.begin(),   m_geometry.normal.begin()   + vertex_count * 3);
    invert_vertex_order_3d(m_geometry.color.begin(),    m_geometry.color.begin()    + vertex_count * 3);
    invert_vertex_order_2d(m_geometry.uv.begin(),       m_geometry.uv.begin()       + vertex_count * 2);

    // SM64 3D vector to Godot 3D vector: (x, y, z) -> (z, y, -x)
    {
        godot::Vector3 *position_ptrw = m_position.ptrw();
        for (int i = 0; i < vertex_count; i++)
        {
            auto &pos = position_ptrw[i];
            auto *geo_pos = &m_geometry.position[3 * i];
            pos.z =   geo_pos[0] / scale_factor;
            pos.y =   geo_pos[1] / scale_factor;
            pos.x =  -geo_pos[2] / scale_factor;
        }
    }

    {
        godot::Vector3 *normal_ptrw = m_normal.ptrw();
        for (int i = 0; i < vertex_count; i++)
        {
            auto &normal = normal_ptrw[i];
            auto *geo_normal = &m_geometry.normal[3 * i];
            normal.z =   geo_normal[0];
            normal.y =   geo_normal[1];
            normal.x =  -geo_normal[2];
        }
    }

    {
        const bool wing_cap_on = m_out_state.flags & 0x8;
        godot::Color *color_ptrw = m_color.ptrw();
        for (int i = 0; i < vertex_count; i++)
        {
            auto &color = color_ptrw[i];
            auto *geo_color = &m_geometry.color[3 * i];
            color.r = geo_color[0];
            color.g = geo_color[1];
            color.b = geo_color[2];
            // Add transparency to the wings of the wing cap (last 24 vertexes)
            if (wing_cap_on && vertex_count > 2256 && i >= vertex_count - 24)
                color.a = 0.0f;
            else
                color.a = 1.0f;
        }
    }

    if constexpr (std::is_same<real_t, float>::value)
    {
        // UV array and Vector2 array have the same memory layout, so we can just copy the data
        memcpy(m_uv.ptrw(), m_geometry.uv.data(), vertex_count * sizeof(godot::Vector2));
    }
    else
    {
        godot::Vector2 *uv_ptrw = m_uv.ptrw();
        for (int i = 0; i < vertex_count; i++)
        {
            auto &uv = uv_ptrw[i];
            auto *geo_uv = &m_geometry.uv[2 * i];
            uv.x = geo_uv[0];
            uv.y = geo_uv[1];
        }
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

    const godot::Vector3 sm64_position = godot::Vector3(p_position.z * scale_factor,
                                                         p_position.y * scale_factor,
                                                        -p_position.x * scale_factor);

    sm64_set_mario_position(m_id, sm64_position.x, sm64_position.y, sm64_position.z);

    m_out_state_hard_tick[m_current_index].position[0] = sm64_position.x;
    m_out_state_hard_tick[m_current_index].position[1] = sm64_position.y;
    m_out_state_hard_tick[m_current_index].position[2] = sm64_position.z;
    reset_interpolation();
    m_reset_interpolation_next_tick = true;
}

void SM64MarioInternal::set_angle(godot::Vector3 p_rotation)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const godot::Vector3 sm64_rotation = godot::Vector3(p_rotation.z,
                                                         p_rotation.y,
                                                        -p_rotation.x);

    sm64_set_mario_angle(m_id, sm64_rotation.x, sm64_rotation.y, sm64_rotation.z);

    m_out_state_hard_tick[m_current_index].faceAngle = sm64_rotation.y;
    reset_interpolation();
    m_reset_interpolation_next_tick = true;
}

void SM64MarioInternal::set_face_angle(real_t p_value)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_set_mario_faceangle(m_id, p_value);

    m_out_state_hard_tick[m_current_index].faceAngle = p_value;
    reset_interpolation();
    m_reset_interpolation_next_tick = true;
}

void SM64MarioInternal::set_velocity(godot::Vector3 p_velocity)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_velocity(m_id,
                             p_velocity.z * g_sm64_delta * scale_factor,
                             p_velocity.y * g_sm64_delta * scale_factor,
                            -p_velocity.x * g_sm64_delta * scale_factor);
}

void SM64MarioInternal::set_forward_velocity(real_t p_velocity)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_forward_velocity(m_id, p_velocity * g_sm64_delta * scale_factor);
}

void SM64MarioInternal::set_invincibility(real_t p_time)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const int16_t time = static_cast<int16_t>(godot::Math::clamp((p_time / g_sm64_delta), (real_t) INT16_MIN, (real_t) INT16_MAX));
    sm64_set_mario_invincibility(m_id, time);
}

void SM64MarioInternal::set_water_level(real_t p_level)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_water_level(m_id, (int) p_level * scale_factor);
}

void SM64MarioInternal::set_gas_level(real_t p_level)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    sm64_set_mario_gas_level(m_id, (int) p_level * scale_factor);
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

void SM64MarioInternal::set_health(int p_health)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const uint16_t health = static_cast<uint16_t>(godot::Math::clamp(p_health, 0, (int) UINT16_MAX));
    sm64_set_mario_health(m_id, health);
}

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

void SM64MarioInternal::heal(int p_wedges)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const int heal_counter = p_wedges * 4;
    sm64_mario_heal(m_id, heal_counter);
}

void SM64MarioInternal::kill()
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    sm64_mario_kill(m_id);
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

    const uint16_t cap_time = static_cast<uint16_t>(godot::Math::clamp((p_cap_time / g_sm64_delta), (real_t) 0, (real_t) UINT16_MAX));
    sm64_mario_interact_cap(m_id, (uint32_t) p_cap, cap_time, (uint8_t) p_play_music);
}

void SM64MarioInternal::extend_cap(real_t p_cap_time)
{
    ERR_FAIL_COND_MSG(m_id < 0, "[libsm64-godot] Non existent Mario");

    const uint16_t cap_time = static_cast<uint16_t>(godot::Math::clamp((p_cap_time / g_sm64_delta), (real_t) 0, (real_t) UINT16_MAX));
    sm64_mario_extend_cap(m_id, cap_time);
}

void SM64MarioInternal::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_interpolate", "value"), &SM64MarioInternal::set_interpolate);
    godot::ClassDB::bind_method(godot::D_METHOD("get_interpolate"), &SM64MarioInternal::get_interpolate);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::BOOL, "interpolate"), "set_interpolate", "get_interpolate");
    godot::ClassDB::bind_method(godot::D_METHOD("reset_interpolation"), &SM64MarioInternal::reset_interpolation);
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
    godot::ClassDB::bind_method(godot::D_METHOD("set_invincibility", "time"), &SM64MarioInternal::set_invincibility);
    godot::ClassDB::bind_method(godot::D_METHOD("set_water_level", "level"), &SM64MarioInternal::set_water_level);
    godot::ClassDB::bind_method(godot::D_METHOD("set_gas_level", "level"), &SM64MarioInternal::set_gas_level);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_floor_override", "surface_properties"), &SM64MarioInternal::set_floor_override);
    // godot::ClassDB::bind_method(godot::D_METHOD("reset_floor_override"), &SM64MarioInternal::reset_floor_override);
    godot::ClassDB::bind_method(godot::D_METHOD("set_health", "health"), &SM64MarioInternal::set_health);
    godot::ClassDB::bind_method(godot::D_METHOD("take_damage", "damage", "source_position", "big_knockback"), &SM64MarioInternal::take_damage, DEFVAL(false));
    godot::ClassDB::bind_method(godot::D_METHOD("heal", "wedges"), &SM64MarioInternal::heal);
    godot::ClassDB::bind_method(godot::D_METHOD("kill"), &SM64MarioInternal::kill);
    // godot::ClassDB::bind_method(godot::D_METHOD("set_lives", "lives"), &SM64MarioInternal::set_lives);
    godot::ClassDB::bind_method(godot::D_METHOD("interact_cap", "cap", "cap_time", "play_music"), &SM64MarioInternal::interact_cap, DEFVAL((real_t)0.0), DEFVAL(true));
    godot::ClassDB::bind_method(godot::D_METHOD("extend_cap", "cap_time"), &SM64MarioInternal::extend_cap);
}
