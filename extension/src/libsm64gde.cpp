#include <libsm64gde.hpp>

#include <cstdlib>
#include <cstring>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#define CONVERT_RADIANS_TO_SM64(x) (-(x) / Math_PI * 32768.0)

static struct SM64TextureAtlasInfo mario_texture_atlas_info = {
    .offset             = 0x114750,
    .numUsedTextures    = 11,
    .atlasWidth         = 11*64,
    .atlasHeight        = 64,
    .texInfos = {
        { .offset = 144, .width = 64, .height = 32, .format = FORMAT_RGBA },
        { .offset = 4240, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 6288, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 8336, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 10384, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 12432, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 14480, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 16528, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 30864, .width = 32, .height = 32, .format = FORMAT_RGBA },
        { .offset = 32912, .width = 32, .height = 64, .format = FORMAT_RGBA },
        { .offset = 37008, .width = 32, .height = 64, .format = FORMAT_RGBA },
    }
};

static void SM64DebugPrintFunction(const char *msg)
{
    godot::UtilityFunctions::print(godot::String("[libsm64] ") + godot::String(msg) + godot::String("\n"));
}

static void invert_vertex_order_2d(float *arr, size_t triangle_count)
{
    for (size_t i = 0; i < triangle_count * 6; i += 6)
    {
        float temp_x = arr[i+0];
        float temp_y = arr[i+1];
        arr[i+0] = arr[i+2];
        arr[i+1] = arr[i+3];
        arr[i+2] = temp_x;
        arr[i+3] = temp_y;
    }
}


static void invert_vertex_order_3d(float *arr, size_t triangle_count)
{
    for (size_t i = 0; i < triangle_count * 9; i += 9)
    {
        float temp_x = arr[i+0];
        float temp_y = arr[i+1];
        float temp_z = arr[i+2];
        arr[i+0] = arr[i+3];
        arr[i+1] = arr[i+4];
        arr[i+2] = arr[i+5];
        arr[i+3] = temp_x;
        arr[i+4] = temp_y;
        arr[i+5] = temp_z;
    }
}

static void invert_vertex_order(godot::PackedVector3Array &arr)
{
    godot::Vector3 *arr_ptrw = arr.ptrw();
    int64_t arr_size = arr.size();

    for (int64_t i = 0; i < arr_size; i += 3)
    {
        godot::Vector3 temp = arr_ptrw[i+0];
        arr_ptrw[i+0] = arr[i+1];
        arr_ptrw[i+1] = temp;
    }
}

SM64::SM64()
{
    ERR_FAIL_COND(singleton != nullptr);
    singleton = this;

    mario_geometry.position = (float*) malloc(sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES);
    mario_geometry.color    = (float*) malloc(sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES);
    mario_geometry.normal   = (float*) malloc(sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES);
    mario_geometry.uv       = (float*) malloc(sizeof(float) * 6 * SM64_GEO_MAX_TRIANGLES);
}

SM64::~SM64()
{
    ERR_FAIL_COND(singleton != this);
    singleton = nullptr;

    global_terminate();

    if (mario_geometry.position)
        ::free(mario_geometry.position);
    if (mario_geometry.color)
        ::free(mario_geometry.color);
    if (mario_geometry.normal)
        ::free(mario_geometry.normal);
    if (mario_geometry.uv)
        ::free(mario_geometry.uv);
}

SM64 *SM64::get_singleton()
{
    return singleton;
}

void SM64::global_init()
{
    const godot::String rom_expected_sha256 = "17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91";
    if (rom_expected_sha256 != godot::FileAccess::get_sha256(rom_filepath))
    {
        godot::UtilityFunctions::print(godot::String("[SM64] ROM file doesnt have expected SHA256: ") + rom_filepath);
        return;
    }

    godot::PackedByteArray rom = godot::FileAccess::get_file_as_bytes(rom_filepath);
    if (rom.is_empty())
    {
        godot::UtilityFunctions::print(godot::String("[SM64] Failed to read ROM file: ") + rom_filepath);
        return;
    }

    constexpr int64_t mario_texture_size = 4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT;
    uint8_t *mario_texture_raw = (uint8_t *) malloc(mario_texture_size);

    sm64_global_init(rom.ptrw(), SM64DebugPrintFunction);
    sm64_texture_load(rom.ptrw(), &mario_texture_atlas_info, mario_texture_raw);

    godot::PackedByteArray mario_texture_packed;
    mario_texture_packed.resize(mario_texture_size);
    memcpy(mario_texture_packed.ptrw(), mario_texture_raw, mario_texture_size);

    mario_image = godot::Image::create_from_data(SM64_TEXTURE_WIDTH, SM64_TEXTURE_HEIGHT, false, godot::Image::FORMAT_RGBA8, mario_texture_packed);
    mario_image_texture = godot::ImageTexture::create_from_image(mario_image);

    init = true;

    ::free(mario_texture_raw);
}

void SM64::global_terminate()
{
    sm64_global_terminate();
    init = false;
}

bool SM64::is_init() const
{
    return init;
}

godot::Ref<godot::Image> SM64::get_mario_image()
{
    return mario_image;
}

godot::Ref<godot::ImageTexture> SM64::get_mario_image_texture()
{
    return mario_image_texture;
}

void SM64::set_rom_filepath(const godot::String &value)
{
    rom_filepath = value;
}

godot::String SM64::get_rom_filepath() const
{
    return rom_filepath;
}

void SM64::set_scale_factor(real_t value)
{
    scale_factor = value;
}

real_t SM64::get_scale_factor() const
{
    return scale_factor;
}

void SM64::static_surfaces_load(godot::PackedVector3Array vertexes, godot::TypedArray<SM64SurfaceProperties> surface_properties_array)
{
    const int64_t vertexes_size = vertexes.size();
    struct SM64Surface *surface_array = (SM64Surface *) malloc(sizeof(SM64Surface) * vertexes_size / 3);
    godot::Ref<SM64SurfaceProperties> default_surface_properties;

    default_surface_properties.instantiate();

    if (surface_properties_array.size() != vertexes_size / 3)
        surface_properties_array.resize(vertexes_size / 3);

    invert_vertex_order(vertexes);

    const godot::Vector3 *vertexes_ptr = vertexes.ptr();
    uint32_t j = 0;
    for (int64_t i = 0; i < vertexes_size; i += 3)
    {
        godot::Ref<SM64SurfaceProperties> surface_properties = surface_properties_array[j];
        if (surface_properties.is_null())
            surface_properties = default_surface_properties;

        surface_array[j].type = (int16_t) surface_properties->get_surface_type();
        surface_array[j].force = (int16_t) surface_properties->get_force();
        surface_array[j].terrain = (uint16_t) surface_properties->get_terrain_type();
        surface_array[j].vertices[0][0] = (int32_t) ( vertexes_ptr[i+0].z * scale_factor);
        surface_array[j].vertices[0][1] = (int32_t) ( vertexes_ptr[i+0].y * scale_factor);
        surface_array[j].vertices[0][2] = (int32_t) (-vertexes_ptr[i+0].x * scale_factor);
        surface_array[j].vertices[1][0] = (int32_t) ( vertexes_ptr[i+1].z * scale_factor);
        surface_array[j].vertices[1][1] = (int32_t) ( vertexes_ptr[i+1].y * scale_factor);
        surface_array[j].vertices[1][2] = (int32_t) (-vertexes_ptr[i+1].x * scale_factor);
        surface_array[j].vertices[2][0] = (int32_t) ( vertexes_ptr[i+2].z * scale_factor);
        surface_array[j].vertices[2][1] = (int32_t) ( vertexes_ptr[i+2].y * scale_factor);
        surface_array[j].vertices[2][2] = (int32_t) (-vertexes_ptr[i+2].x * scale_factor);

        j++;
    }

    sm64_static_surfaces_load(surface_array, j);

    ::free(surface_array);
}

int SM64::mario_create(godot::Vector3 position, godot::Vector3 rotation)
{
    float x = (float) ( position.z * scale_factor);
    float y = (float) ( position.y * scale_factor);
    float z = (float) (-position.x * scale_factor);
    int16_t rx = (int16_t) CONVERT_RADIANS_TO_SM64(-rotation.z);
    int16_t ry = (int16_t) CONVERT_RADIANS_TO_SM64(-rotation.y);
    int16_t rz = (int16_t) CONVERT_RADIANS_TO_SM64( rotation.x);

    return sm64_mario_create(x, y, z, rx, ry, rz, 0);
}

godot::Dictionary SM64::mario_tick(int mario_id, godot::Dictionary input)
{
    godot::Dictionary ret;
    godot::Array mesh_array;
    struct SM64MarioState out_state;

    godot::Vector2 cam_look = input["cam_look"];
    godot::Vector2 stick = input["stick"];
    bool a = input["a"];
    bool b = input["b"];
    bool z = input["z"];

    struct SM64MarioInputs mario_inputs = {
        -cam_look.y, // camLookX
        cam_look.x, // camLookZ
        stick.x,
        stick.y,
        (uint8_t) a,
        (uint8_t) b,
        (uint8_t) z
    };

    sm64_mario_tick(mario_id, &mario_inputs, &out_state, &mario_geometry);

    ret["position"]       = godot::Vector3(-out_state.position[2] / scale_factor, out_state.position[1] / scale_factor, out_state.position[0] / scale_factor);
    ret["velocity"]       = godot::Vector3(-out_state.velocity[2] / scale_factor, out_state.velocity[1] / scale_factor, out_state.velocity[0] / scale_factor);
    ret["face_angle"]     = (real_t) out_state.faceAngle;
    ret["health"]         = (int) out_state.health;
    ret["action"]         = (int) out_state.action;
    ret["flags"]          = (int) out_state.flags;
    ret["particle_flags"] = (int) out_state.particleFlags;
    ret["invinc_timer"]   = (int) out_state.invincTimer;
    ret["hurt_counter"]   = (int) out_state.hurtCounter;
    ret["num_lives"]      = (int) out_state.numLives;
    ret["holding_object"] = (bool) out_state.holdingObject;
    ret["drop_method"]    = (int) out_state.dropMethod;

    const int vertex_count = mario_geometry.numTrianglesUsed * 3;
    if (mesh_array.size() != godot::ArrayMesh::ARRAY_MAX)
        mesh_array.resize(godot::ArrayMesh::ARRAY_MAX);
    if (mario_position.size() != vertex_count)
        mario_position.resize(vertex_count);
    if (mario_normal.size() != vertex_count)
        mario_normal.resize(vertex_count);
    if (mario_color.size() != vertex_count)
        mario_color.resize(vertex_count);
    if (mario_uv.size() != vertex_count)
        mario_uv.resize(vertex_count);

    invert_vertex_order_3d(mario_geometry.position, mario_geometry.numTrianglesUsed);
    invert_vertex_order_3d(mario_geometry.normal,   mario_geometry.numTrianglesUsed);
    invert_vertex_order_3d(mario_geometry.color,    mario_geometry.numTrianglesUsed);
    invert_vertex_order_2d(mario_geometry.uv,       mario_geometry.numTrianglesUsed);

    godot::Vector3 *mario_position_ptrw = mario_position.ptrw();
    godot::Vector3 *mario_normal_ptrw   = mario_normal.ptrw();
    godot::Color   *mario_color_ptrw    = mario_color.ptrw();
    godot::Vector2 *mario_uv_ptrw       = mario_uv.ptrw();
    for (int i = 0; i < vertex_count; i += 3)
    {
        mario_position_ptrw[i+0].z =  mario_geometry.position[3*i+0] / scale_factor;
        mario_position_ptrw[i+0].y =  mario_geometry.position[3*i+1] / scale_factor;
        mario_position_ptrw[i+0].x = -mario_geometry.position[3*i+2] / scale_factor;
        mario_position_ptrw[i+1].z =  mario_geometry.position[3*i+3] / scale_factor;
        mario_position_ptrw[i+1].y =  mario_geometry.position[3*i+4] / scale_factor;
        mario_position_ptrw[i+1].x = -mario_geometry.position[3*i+5] / scale_factor;
        mario_position_ptrw[i+2].z =  mario_geometry.position[3*i+6] / scale_factor;
        mario_position_ptrw[i+2].y =  mario_geometry.position[3*i+7] / scale_factor;
        mario_position_ptrw[i+2].x = -mario_geometry.position[3*i+8] / scale_factor;

        mario_normal_ptrw[i+0].z =  mario_geometry.normal[3*i+0];
        mario_normal_ptrw[i+0].y =  mario_geometry.normal[3*i+1];
        mario_normal_ptrw[i+0].x = -mario_geometry.normal[3*i+2];
        mario_normal_ptrw[i+1].z =  mario_geometry.normal[3*i+3];
        mario_normal_ptrw[i+1].y =  mario_geometry.normal[3*i+4];
        mario_normal_ptrw[i+1].x = -mario_geometry.normal[3*i+5];
        mario_normal_ptrw[i+2].z =  mario_geometry.normal[3*i+6];
        mario_normal_ptrw[i+2].y =  mario_geometry.normal[3*i+7];
        mario_normal_ptrw[i+2].x = -mario_geometry.normal[3*i+8];

        mario_color_ptrw[i+0] = godot::Color(mario_geometry.color[3*i+0], mario_geometry.color[3*i+1], mario_geometry.color[3*i+2]);
        mario_color_ptrw[i+1] = godot::Color(mario_geometry.color[3*i+3], mario_geometry.color[3*i+4], mario_geometry.color[3*i+5]);
        mario_color_ptrw[i+2] = godot::Color(mario_geometry.color[3*i+6], mario_geometry.color[3*i+7], mario_geometry.color[3*i+8]);

        mario_uv_ptrw[i+0] = godot::Vector2(mario_geometry.uv[2*i+0], mario_geometry.uv[2*i+1]);
        mario_uv_ptrw[i+1] = godot::Vector2(mario_geometry.uv[2*i+2], mario_geometry.uv[2*i+3]);
        mario_uv_ptrw[i+2] = godot::Vector2(mario_geometry.uv[2*i+4], mario_geometry.uv[2*i+5]);
    }

    mesh_array[godot::ArrayMesh::ARRAY_VERTEX] = mario_position;
    mesh_array[godot::ArrayMesh::ARRAY_NORMAL] = mario_normal;
    mesh_array[godot::ArrayMesh::ARRAY_COLOR]  = mario_color;
    mesh_array[godot::ArrayMesh::ARRAY_TEX_UV] = mario_uv;

    ret["mesh_array"] = mesh_array;

    return ret;
}

void SM64::mario_delete(int mario_id)
{
    sm64_mario_delete(mario_id);
}

void SM64::set_mario_action(int mario_id, int action)
{
    sm64_set_mario_action(mario_id, (uint32_t) action);
}

void SM64::set_mario_state(int mario_id, int flags)
{
    sm64_set_mario_state(mario_id, (uint32_t) flags);
}

void SM64::set_mario_position(int mario_id, godot::Vector3 position)
{
    sm64_set_mario_position(mario_id,
                            position.z * scale_factor,
                            position.y * scale_factor,
                            -position.x * scale_factor);
}

void SM64::set_mario_angle(int mario_id, real_t angle)
{
    sm64_set_mario_angle(mario_id, angle);
}

void SM64::set_mario_velocity(int mario_id, godot::Vector3 velocity)
{
    sm64_set_mario_velocity(mario_id,
                            velocity.z * scale_factor,
                            velocity.y * scale_factor,
                            -velocity.x * scale_factor);
}

void SM64::set_mario_forward_velocity(int mario_id, real_t velocity)
{
    sm64_set_mario_forward_velocity(mario_id, velocity * scale_factor);
}

void SM64::set_mario_invincibility(int mario_id, int timer)
{
    sm64_set_mario_invincibility(mario_id, timer);
}

void SM64::set_mario_water_level(int mario_id, real_t level)
{
    sm64_set_mario_water_level(mario_id, level * scale_factor);
}

void SM64::set_mario_floor_override(int mario_id, godot::Ref<SM64SurfaceProperties> surface_properties)
{
    if (surface_properties.is_null())
    {
        godot::UtilityFunctions::print(godot::String("[SM64] Called set_mario_floor_override with null surface_properties"));
        return;
    }

    sm64_set_mario_floor_override(mario_id,
                                  surface_properties->get_terrain_type(),
                                  surface_properties->get_surface_type(),
                                  surface_properties->get_force());
}

void SM64::reset_mario_floor_override(int mario_id)
{
    sm64_set_mario_floor_override(mario_id, 0x7, 0x100, 0);
}

void SM64::set_mario_health(int mario_id, int health)
{
    sm64_set_mario_health(mario_id, health);
}

void SM64::mario_take_damage(int mario_id, int damage, godot::Vector3 source_position, bool big_knockback)
{
    sm64_mario_take_damage(mario_id,
                           damage,
                           big_knockback ? 0x8 : 0x0,
                           source_position.z * scale_factor,
                           source_position.y * scale_factor,
                           -source_position.x * scale_factor);
}

void SM64::mario_heal(int mario_id, int heal_counter)
{
    sm64_mario_heal(mario_id, heal_counter);
}

void SM64::mario_set_lives(int mario_id, int lives)
{
    sm64_mario_set_lives(mario_id, lives);
}

void SM64::mario_interact_cap(int mario_id, int cap, int cap_time, bool play_music)
{
    sm64_mario_interact_cap(mario_id, (uint32_t) cap, cap_time, (uint8_t) play_music);
}

void SM64::mario_extend_cap(int mario_id, int cap_time)
{
    sm64_mario_extend_cap(mario_id, cap_time);
}

int SM64::surface_object_create(godot::PackedVector3Array vertexes, godot::Vector3 position, godot::Vector3 rotation, godot::TypedArray<SM64SurfaceProperties> surface_properties_array)
{
    const int64_t vertexes_size = vertexes.size();
    struct SM64SurfaceObject surface_object;
    int id;
    struct SM64Surface *surface_array = (SM64Surface *) malloc(sizeof(SM64Surface) * vertexes_size / 3);
    godot::Ref<SM64SurfaceProperties> default_surface_properties;

    default_surface_properties.instantiate();

    if (surface_properties_array.size() != vertexes_size / 3)
        surface_properties_array.resize(vertexes_size / 3);

    invert_vertex_order(vertexes);

    const godot::Vector3 *vertexes_ptr = vertexes.ptr();
    uint32_t j = 0;
    for (int64_t i = 0; i < vertexes_size; i += 3)
    {
        godot::Ref<SM64SurfaceProperties> surface_properties = surface_properties_array[j];
        if (surface_properties.is_null())
            surface_properties = default_surface_properties;

        surface_array[j].type = (int16_t) surface_properties->get_surface_type();
        surface_array[j].force = (int16_t) surface_properties->get_force();
        surface_array[j].terrain = (uint16_t) surface_properties->get_terrain_type();
        surface_array[j].vertices[0][0] = (int32_t) ( vertexes_ptr[i+0].z * scale_factor);
        surface_array[j].vertices[0][1] = (int32_t) ( vertexes_ptr[i+0].y * scale_factor);
        surface_array[j].vertices[0][2] = (int32_t) (-vertexes_ptr[i+0].x * scale_factor);
        surface_array[j].vertices[1][0] = (int32_t) ( vertexes_ptr[i+1].z * scale_factor);
        surface_array[j].vertices[1][1] = (int32_t) ( vertexes_ptr[i+1].y * scale_factor);
        surface_array[j].vertices[1][2] = (int32_t) (-vertexes_ptr[i+1].x * scale_factor);
        surface_array[j].vertices[2][0] = (int32_t) ( vertexes_ptr[i+2].z * scale_factor);
        surface_array[j].vertices[2][1] = (int32_t) ( vertexes_ptr[i+2].y * scale_factor);
        surface_array[j].vertices[2][2] = (int32_t) (-vertexes_ptr[i+2].x * scale_factor);

        j++;
    }

    surface_object.surfaces = surface_array;
    surface_object.surfaceCount = j;

    surface_object.transform.position[0] =  position.z * scale_factor;
    surface_object.transform.position[1] =  position.y * scale_factor;
    surface_object.transform.position[2] = -position.x * scale_factor;

    surface_object.transform.eulerRotation[0] = -godot::Math::rad_to_deg(rotation.z);
    surface_object.transform.eulerRotation[1] = -godot::Math::rad_to_deg(rotation.y);
    surface_object.transform.eulerRotation[2] =  godot::Math::rad_to_deg(rotation.x);

    id = sm64_surface_object_create(&surface_object);

    ::free(surface_array);

    return id;
}

void SM64::surface_object_move(int object_id, godot::Vector3 position, godot::Vector3 rotation)
{
    struct SM64ObjectTransform transform;

    transform.position[0] =  position.z * scale_factor;
    transform.position[1] =  position.y * scale_factor;
    transform.position[2] = -position.x * scale_factor;

    transform.eulerRotation[0] = -godot::Math::rad_to_deg(rotation.z);
    transform.eulerRotation[1] = -godot::Math::rad_to_deg(rotation.y);
    transform.eulerRotation[2] =  godot::Math::rad_to_deg(rotation.x);

    sm64_surface_object_move(object_id, &transform);
}

void SM64::surface_object_delete(int object_id)
{
    sm64_surface_object_delete(object_id);
}

void SM64::set_volume(real_t volume)
{
    sm64_set_volume(volume);
}

void SM64::set_reverb(int reverb)
{
    sm64_set_reverb((uint8_t) reverb);
}

void SM64::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("global_init"), &SM64::global_init);
    godot::ClassDB::bind_method(godot::D_METHOD("global_terminate"), &SM64::global_terminate);
    godot::ClassDB::bind_method(godot::D_METHOD("is_init"), &SM64::is_init);
    godot::ClassDB::bind_method(godot::D_METHOD("get_mario_image"), &SM64::get_mario_image);
    godot::ClassDB::bind_method(godot::D_METHOD("get_mario_image_texture"), &SM64::get_mario_image_texture);
    godot::ClassDB::bind_method(godot::D_METHOD("set_rom_filepath", "value"), &SM64::set_rom_filepath);
    godot::ClassDB::bind_method(godot::D_METHOD("get_rom_filepath"), &SM64::get_rom_filepath);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::STRING, "rom_filepath", godot::PROPERTY_HINT_GLOBAL_FILE, "*.n64,*.z64,"), "set_rom_filepath", "get_rom_filepath");
    godot::ClassDB::bind_method(godot::D_METHOD("set_scale_factor", "value"), &SM64::set_scale_factor);
    godot::ClassDB::bind_method(godot::D_METHOD("get_scale_factor"), &SM64::get_scale_factor);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "scale_factor"), "set_scale_factor", "get_scale_factor");
    godot::ClassDB::bind_method(godot::D_METHOD("static_surfaces_load", "vertexes", "surface_properties_array"), &SM64::static_surfaces_load);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_create", "position", "rotation"), &SM64::mario_create);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_tick", "mario_id", "input"), &SM64::mario_tick);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_delete", "mario_id"), &SM64::mario_delete);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_action", "mario_id", "action"), &SM64::set_mario_action);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_state", "mario_id", "flags"), &SM64::set_mario_state);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_position", "mario_id", "position"), &SM64::set_mario_position);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_angle", "mario_id", "angle"), &SM64::set_mario_angle);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_velocity", "mario_id", "velocity"), &SM64::set_mario_velocity);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_forward_velocity", "mario_id", "velocity"), &SM64::set_mario_forward_velocity);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_invincibility", "mario_id", "timer"), &SM64::set_mario_invincibility);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_water_level", "mario_id", "level"), &SM64::set_mario_water_level);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_floor_override", "mario_id", "surface_properties"), &SM64::set_mario_floor_override);
    godot::ClassDB::bind_method(godot::D_METHOD("reset_mario_floor_override", "mario_id"), &SM64::reset_mario_floor_override);
    godot::ClassDB::bind_method(godot::D_METHOD("set_mario_health", "mario_id", "health"), &SM64::set_mario_health);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_take_damage", "mario_id", "damage", "source_position", "big_knockback"), &SM64::mario_take_damage, DEFVAL(false));
    godot::ClassDB::bind_method(godot::D_METHOD("mario_heal", "mario_id", "heal_counter"), &SM64::mario_heal);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_set_lives", "mario_id", "lives"), &SM64::mario_set_lives);
    godot::ClassDB::bind_method(godot::D_METHOD("mario_interact_cap", "mario_id", "cap", "cap_time", "play_music"), &SM64::mario_interact_cap, DEFVAL(0), DEFVAL(true));
    godot::ClassDB::bind_method(godot::D_METHOD("mario_extend_cap", "mario_id", "cap_time"), &SM64::mario_extend_cap);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_create", "vertexes", "position", "rotation", "surface_properties_array"), &SM64::surface_object_create);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_move", "object_id", "position", "rotation"), &SM64::surface_object_move);
    godot::ClassDB::bind_method(godot::D_METHOD("surface_object_delete", "object_id"), &SM64::surface_object_delete);
    godot::ClassDB::bind_method(godot::D_METHOD("set_volume", "volume"), &SM64::set_volume);
    godot::ClassDB::bind_method(godot::D_METHOD("set_reverb", "reverb"), &SM64::set_reverb);
}
