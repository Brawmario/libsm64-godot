#include <libsm64_mario_interpolator.hpp>

#include <godot_cpp/classes/array_mesh.hpp>

static godot::PackedVector3Array lerp(const godot::Vector3 *a, const godot::Vector3 *b, int64_t vertex_count, real_t t)
{
    auto ret = godot::PackedVector3Array();
    ret.resize(vertex_count);

    auto *ret_ptrw = ret.ptrw();
    for (int i = 0; i < vertex_count; i++)
    {
        ret_ptrw[i] = a[i].lerp(b[i], t);
    }

    return ret;
}

LibSM64MarioInterpolator::LibSM64MarioInterpolator()
{
    array_mesh_triangles_current.resize(godot::ArrayMesh::ARRAY_MAX);
    array_mesh_triangles_current[godot::ArrayMesh::ARRAY_VERTEX] = godot::PackedVector3Array();
    array_mesh_triangles_current[godot::ArrayMesh::ARRAY_NORMAL] = godot::PackedVector3Array();
    array_mesh_triangles_current[godot::ArrayMesh::ARRAY_COLOR] = godot::PackedColorArray();
    array_mesh_triangles_current[godot::ArrayMesh::ARRAY_TEX_UV] = godot::PackedVector2Array();
    array_mesh_triangles_previous.resize(godot::ArrayMesh::ARRAY_MAX);
    array_mesh_triangles_previous[godot::ArrayMesh::ARRAY_VERTEX] = godot::PackedVector3Array();
    array_mesh_triangles_previous[godot::ArrayMesh::ARRAY_NORMAL] = godot::PackedVector3Array();
    array_mesh_triangles_previous[godot::ArrayMesh::ARRAY_COLOR] = godot::PackedColorArray();
    array_mesh_triangles_previous[godot::ArrayMesh::ARRAY_TEX_UV] = godot::PackedVector2Array();

    mario_state_current.instantiate();
    mario_state_previous.instantiate();
}

void LibSM64MarioInterpolator::set_array_mesh_triangles_current(const godot::Array &p_value)
{
    array_mesh_triangles_current = p_value;
}

godot::Array LibSM64MarioInterpolator::get_array_mesh_triangles_current() const
{
    return array_mesh_triangles_current;
}

void LibSM64MarioInterpolator::set_array_mesh_triangles_previous(const godot::Array &p_value)
{
    array_mesh_triangles_previous = p_value;
}

godot::Array LibSM64MarioInterpolator::get_array_mesh_triangles_previous() const
{
    return array_mesh_triangles_previous;
}

void LibSM64MarioInterpolator::set_mario_state_current(const godot::Ref<LibSM64MarioState> &p_value)
{
    mario_state_current = p_value;
}

godot::Ref<LibSM64MarioState> LibSM64MarioInterpolator::get_mario_state_current() const
{
    return mario_state_current;
}

void LibSM64MarioInterpolator::set_mario_state_previous(const godot::Ref<LibSM64MarioState> &p_value)
{
    mario_state_previous = p_value;
}

godot::Ref<LibSM64MarioState> LibSM64MarioInterpolator::get_mario_state_previous() const
{
    return mario_state_previous;
}

godot::Array LibSM64MarioInterpolator::interpolate_array_mesh_triangles(double p_t) const
{
    auto ret = godot::Array();
    ret.resize(godot::ArrayMesh::ARRAY_MAX);

    const godot::PackedVector3Array vertices_current = array_mesh_triangles_current[godot::ArrayMesh::ARRAY_VERTEX];
    const godot::PackedVector3Array vertices_previous = array_mesh_triangles_previous[godot::ArrayMesh::ARRAY_VERTEX];
    const godot::PackedVector3Array normals_current = array_mesh_triangles_current[godot::ArrayMesh::ARRAY_NORMAL];
    const godot::PackedVector3Array normals_previous = array_mesh_triangles_previous[godot::ArrayMesh::ARRAY_NORMAL];

    auto vertex_count = godot::MIN(vertices_current.size(), vertices_previous.size());

    ret[godot::ArrayMesh::ARRAY_VERTEX] = lerp(vertices_previous.ptr(), vertices_current.ptr(), vertex_count, static_cast<real_t>(p_t));
    ret[godot::ArrayMesh::ARRAY_NORMAL] = lerp(normals_previous.ptr(), normals_current.ptr(), vertex_count, static_cast<real_t>(p_t));

    const godot::PackedColorArray colors_current = array_mesh_triangles_current[godot::ArrayMesh::ARRAY_COLOR];
    const godot::PackedVector2Array uvs_current = array_mesh_triangles_current[godot::ArrayMesh::ARRAY_TEX_UV];

    ret[godot::ArrayMesh::ARRAY_COLOR] = colors_current.slice(0, vertex_count);
    ret[godot::ArrayMesh::ARRAY_TEX_UV] = uvs_current.slice(0, vertex_count);

    return ret;
}

godot::Ref<LibSM64MarioState> LibSM64MarioInterpolator::interpolate_mario_state(double p_t) const
{
    ERR_FAIL_NULL_V(mario_state_current, nullptr);
    ERR_FAIL_NULL_V(mario_state_previous, nullptr);

    godot::Ref<LibSM64MarioState> ret = memnew(LibSM64MarioState);

    ret->position = mario_state_previous->position.lerp(mario_state_current->position, p_t);
    ret->velocity = mario_state_previous->velocity.lerp(mario_state_current->velocity, p_t);
    ret->face_angle = godot::Math::lerp_angle(mario_state_previous->face_angle, mario_state_current->face_angle, static_cast<float>(p_t));

    ret->health = mario_state_current->health;
    ret->action = mario_state_current->action;
    ret->flags = mario_state_current->flags;
    ret->particle_flags = mario_state_current->particle_flags;

    ret->invincibility_time = godot::MAX(0.0, godot::Math::lerp(mario_state_previous->invincibility_time, mario_state_current->invincibility_time, p_t));

    return ret;
}

void LibSM64MarioInterpolator::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_array_mesh_triangles_current", "value"), &LibSM64MarioInterpolator::set_array_mesh_triangles_current);
    godot::ClassDB::bind_method(godot::D_METHOD("get_array_mesh_triangles_current"), &LibSM64MarioInterpolator::get_array_mesh_triangles_current);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::ARRAY, "array_mesh_triangles_current"), "set_array_mesh_triangles_current", "get_array_mesh_triangles_current");
    godot::ClassDB::bind_method(godot::D_METHOD("set_array_mesh_triangles_previous", "value"), &LibSM64MarioInterpolator::set_array_mesh_triangles_previous);
    godot::ClassDB::bind_method(godot::D_METHOD("get_array_mesh_triangles_previous"), &LibSM64MarioInterpolator::get_array_mesh_triangles_previous);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::ARRAY, "array_mesh_triangles_previous"), "set_array_mesh_triangles_previous", "get_array_mesh_triangles_previous");
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_state_current", "value"), &LibSM64MarioInterpolator::set_mario_state_current);
    godot::ClassDB::bind_method(godot::D_METHOD("get_mario_state_current"), &LibSM64MarioInterpolator::get_mario_state_current);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "mario_state_current"), "set_mario_state_current", "get_mario_state_current");
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_state_previous", "value"), &LibSM64MarioInterpolator::set_mario_state_previous);
    godot::ClassDB::bind_method(godot::D_METHOD("get_mario_state_previous"), &LibSM64MarioInterpolator::get_mario_state_previous);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "mario_state_previous"), "set_mario_state_previous", "get_mario_state_previous");
    godot::ClassDB::bind_method(godot::D_METHOD("interpolate_array_mesh_triangles", "t"), &LibSM64MarioInterpolator::interpolate_array_mesh_triangles);
    godot::ClassDB::bind_method(godot::D_METHOD("interpolate_mario_state", "t"), &LibSM64MarioInterpolator::interpolate_mario_state);
}
