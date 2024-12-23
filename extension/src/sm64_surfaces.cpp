#include <sm64_surfaces.hpp>

#include <cstdlib>
#include <cstring>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <sm64_global.hpp>

_FORCE_INLINE_ static void invert_vertex_order(godot::PackedVector3Array &p_arr)
{
    godot::Vector3 *vertexes = p_arr.ptrw();
    int64_t vertexes_size = p_arr.size();
    for (int i = 0; i < vertexes_size; i += 3)
    {
        std::swap(vertexes[i], vertexes[i + 1]);
    }
}

SM64Surfaces::SM64Surfaces()
{
    ERR_FAIL_COND(singleton != nullptr);
    singleton = this;
}

SM64Surfaces::~SM64Surfaces()
{
    ERR_FAIL_COND(singleton != this);
    singleton = nullptr;
}

SM64Surfaces *SM64Surfaces::get_singleton()
{
    return singleton;
}

void SM64Surfaces::static_surfaces_load(godot::PackedVector3Array p_vertexes, godot::TypedArray<SM64SurfaceProperties> p_surface_properties_array)
{
    ERR_FAIL_COND_MSG(p_vertexes.size() % 3 != 0, "[libsm64-godot] Vertex array size must be a multiple of 3");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] Called static_surfaces_load but SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();
    const int64_t vertexes_size = p_vertexes.size();
    struct SM64Surface *surface_array = (SM64Surface *) malloc(sizeof(SM64Surface) * vertexes_size / 3);
    godot::Ref<SM64SurfaceProperties> default_surface_properties;

    default_surface_properties.instantiate();

    p_surface_properties_array.resize(vertexes_size / 3);

    invert_vertex_order(p_vertexes);

    godot::Vector3 *vertexes_ptrw = p_vertexes.ptrw();
    uint32_t si;
    for (si = 0; si < vertexes_size / 3; si++)
    {
        godot::Ref<SM64SurfaceProperties> surface_properties = p_surface_properties_array[si];
        if (surface_properties.is_null())
            surface_properties = default_surface_properties;

        surface_array[si].type = (int16_t) surface_properties->get_surface_type();
        surface_array[si].force = (int16_t) surface_properties->get_force();
        surface_array[si].terrain = (uint16_t) surface_properties->get_terrain_type();

        for (int sj = 0; sj < 3; sj++)
        {
            int32_t *sa_vertex = (int32_t *) &surface_array[si].vertices[sj];
            auto &vec3 = vertexes_ptrw[si * 3 + sj];
            sa_vertex[0] = (int32_t) ( vec3.z * scale_factor);
            sa_vertex[1] = (int32_t) ( vec3.y * scale_factor);
            sa_vertex[2] = (int32_t) (-vec3.x * scale_factor);
        }
    }

    sm64_static_surfaces_load(surface_array, si);

    ::free(surface_array);
}

int SM64Surfaces::surface_object_create(godot::PackedVector3Array p_vertexes, godot::Vector3 p_position, godot::Vector3 p_rotation, godot::TypedArray<SM64SurfaceProperties> p_surface_properties_array)
{
    ERR_FAIL_COND_V_MSG(p_vertexes.size() % 3 != 0, -1, "[libsm64-godot] Vertex array size must be a multiple of 3");

    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL_V(sm64_global, -1);
    ERR_FAIL_COND_V_MSG(!sm64_global->is_init(), -1, "[libsm64-godot] Called surface_object_create but SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();
    const int64_t vertexes_size = p_vertexes.size();
    struct SM64SurfaceObject surface_object;
    int id;
    struct SM64Surface *surface_array = (SM64Surface *) malloc(sizeof(SM64Surface) * vertexes_size / 3);
    godot::Ref<SM64SurfaceProperties> default_surface_properties;

    default_surface_properties.instantiate();

    p_surface_properties_array.resize(vertexes_size / 3);

    invert_vertex_order(p_vertexes);

    godot::Vector3 *vertexes_ptrw = p_vertexes.ptrw();
    uint32_t si;
    for (si = 0; si < vertexes_size / 3; si++)
    {
        godot::Ref<SM64SurfaceProperties> surface_properties = p_surface_properties_array[si];
        if (surface_properties.is_null())
            surface_properties = default_surface_properties;

        surface_array[si].type = (int16_t) surface_properties->get_surface_type();
        surface_array[si].force = (int16_t) surface_properties->get_force();
        surface_array[si].terrain = (uint16_t) surface_properties->get_terrain_type();

        for (int sj = 0; sj < 3; sj++)
        {
            int32_t *sa_vertex = (int32_t *) &surface_array[si].vertices[sj];
            auto &vec3 = vertexes_ptrw[si * 3 + sj];
            sa_vertex[0] = (int32_t) ( vec3.z * scale_factor);
            sa_vertex[1] = (int32_t) ( vec3.y * scale_factor);
            sa_vertex[2] = (int32_t) (-vec3.x * scale_factor);
        }
    }

    surface_object.surfaces = surface_array;
    surface_object.surfaceCount = si;

    surface_object.transform.position[0] =  p_position.z * scale_factor;
    surface_object.transform.position[1] =  p_position.y * scale_factor;
    surface_object.transform.position[2] = -p_position.x * scale_factor;

    surface_object.transform.eulerRotation[0] = -godot::Math::rad_to_deg(p_rotation.z);
    surface_object.transform.eulerRotation[1] = -godot::Math::rad_to_deg(p_rotation.y);
    surface_object.transform.eulerRotation[2] =  godot::Math::rad_to_deg(p_rotation.x);

    id = sm64_surface_object_create(&surface_object);

    ::free(surface_array);

    return id;
}

void SM64Surfaces::surface_object_move(int p_object_id, godot::Vector3 p_position, godot::Vector3 p_rotation)
{
    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] Called surface_object_move but SM64Global is not init");

    const real_t scale_factor = sm64_global->get_scale_factor();

    struct SM64ObjectTransform transform;

    transform.position[0] =  p_position.z * scale_factor;
    transform.position[1] =  p_position.y * scale_factor;
    transform.position[2] = -p_position.x * scale_factor;

    transform.eulerRotation[0] = -godot::Math::rad_to_deg(p_rotation.z);
    transform.eulerRotation[1] = -godot::Math::rad_to_deg(p_rotation.y);
    transform.eulerRotation[2] =  godot::Math::rad_to_deg(p_rotation.x);

    sm64_surface_object_move(p_object_id, &transform);
}

void SM64Surfaces::surface_object_delete(int p_object_id)
{
    const SM64Global *sm64_global = SM64Global::get_singleton();
    ERR_FAIL_NULL(sm64_global);
    ERR_FAIL_COND_MSG(!sm64_global->is_init(), "[libsm64-godot] Called surface_object_delete but SM64Global is not init");

    sm64_surface_object_delete(p_object_id);
}


void SM64Surfaces::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("static_surfaces_load", "vertexes", "surface_properties_array"), &SM64Surfaces::static_surfaces_load);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_create", "vertexes", "position", "rotation", "surface_properties_array"), &SM64Surfaces::surface_object_create);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_move", "object_id", "position", "rotation"), &SM64Surfaces::surface_object_move);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_delete", "object_id"), &SM64Surfaces::surface_object_delete);
}
