#include <SM64Handler.hpp>

#include <string>
#include <cstdio>

#include<ArrayMesh.hpp>

static constexpr real_t SM64_SCALE_FACTOR = 50;

static void SM64DebugPrintFunction(const char *msg)
{
    godot::Godot::print(godot::String("[libsm64] ") + godot::String(msg) + godot::String("\n"));
}

static uint8_t *utils_read_file_alloc(const char *path, size_t *fileLength)
{
    FILE *f = fopen(path, "rb");

    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    size_t length = (size_t)ftell(f);
    rewind(f);
    uint8_t *buffer = (uint8_t *)malloc(length + 1);
    fread(buffer, 1, length, f);
    buffer[length] = 0;
    fclose(f);

    if (fileLength)
        *fileLength = length;

    return buffer;
}

static bool check_in_bounds(real_t num)
{
    constexpr real_t bounds = 0x7FFF;
    return (num > -bounds && num < bounds);
}

static bool check_in_bounds(godot::Vector3 vec)
{
    vec *= SM64_SCALE_FACTOR;
    return check_in_bounds(vec.x) && check_in_bounds(vec.y) && check_in_bounds(vec.z);
}

static void invert_vertex_order(float *arr, size_t triangle_count)
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

static void invert_vertex_order(godot::PoolVector3Array &arr)
{
    for (size_t i = 0; i < arr.size() / 3; i++)
    {
        godot::Vector3 temp = arr[3*i+0];
        arr.set(3*i+0, arr[3*i+1]);
        arr.set(3*i+1, temp);
    }
}

SM64Handler::SM64Handler()
{
}

void SM64Handler::_init()
{
    mario_geometry.position = (float*) malloc(sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES);
    mario_geometry.color    = (float*) malloc(sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES);
    mario_geometry.normal   = (float*) malloc(sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES);
    mario_geometry.uv       = (float*) malloc(sizeof(float) * 6 * SM64_GEO_MAX_TRIANGLES);
}

SM64Handler::~SM64Handler()
{
    sm64_global_terminate();

    if (mario_geometry.position)
        ::free(mario_geometry.position);
    if (mario_geometry.color)
        ::free(mario_geometry.color);
    if (mario_geometry.normal)
        ::free(mario_geometry.normal);
    if (mario_geometry.uv)
        ::free(mario_geometry.uv);
}

godot::Ref<godot::Image> SM64Handler::global_init(godot::String rom_filename)
{
    size_t rom_size;
    uint8_t *rom = utils_read_file_alloc(rom_filename.ascii().get_data(), &rom_size);

    if (rom == NULL)
    {
        godot::Godot::print(godot::String("[SM64Handler] Failed to read ROM file: ") + rom_filename);
        return nullptr;
    }

    uint8_t *mario_texture = (uint8_t *) malloc(4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT);

    sm64_global_init(rom, mario_texture, SM64DebugPrintFunction);

    godot::PoolByteArray mario_texure_pool;
    mario_texure_pool.resize(4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT);
    for (size_t i = 0; i < 4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT; i++)
        mario_texure_pool.set(i, mario_texture[i]);
    
    godot::Ref<godot::Image> mario_image = godot::Image::_new();
    mario_image->create_from_data(SM64_TEXTURE_WIDTH, SM64_TEXTURE_HEIGHT, false, godot::Image::FORMAT_RGBA8, mario_texure_pool);

    ::free(rom);
    ::free(mario_texture);

    return mario_image;
}

void SM64Handler::static_surfaces_load(godot::PoolVector3Array vertexes)
{
    struct SM64Surface *surface_array = (SM64Surface *) malloc(sizeof(SM64Surface) * vertexes.size() / 3);

    invert_vertex_order(vertexes);

    uint32_t j = 0;
    for (size_t i = 0; i < vertexes.size(); i += 3)
    {
        if (!check_in_bounds(vertexes[i]) || !check_in_bounds(vertexes[i+1]) || !check_in_bounds(vertexes[i+2]))
            continue;

        surface_array[j].type = 0x0000; // SURFACE_DEFAULT
        surface_array[j].force = 0;
        surface_array[j].terrain = 0x0002; // TERRAIN_SNOW
        surface_array[j].vertices[0][0] = (int16_t) ( vertexes[i+0].z * SM64_SCALE_FACTOR);
        surface_array[j].vertices[0][1] = (int16_t) ( vertexes[i+0].y * SM64_SCALE_FACTOR);
        surface_array[j].vertices[0][2] = (int16_t) (-vertexes[i+0].x * SM64_SCALE_FACTOR);
        surface_array[j].vertices[1][0] = (int16_t) ( vertexes[i+1].z * SM64_SCALE_FACTOR);
        surface_array[j].vertices[1][1] = (int16_t) ( vertexes[i+1].y * SM64_SCALE_FACTOR);
        surface_array[j].vertices[1][2] = (int16_t) (-vertexes[i+1].x * SM64_SCALE_FACTOR);
        surface_array[j].vertices[2][0] = (int16_t) ( vertexes[i+2].z * SM64_SCALE_FACTOR);
        surface_array[j].vertices[2][1] = (int16_t) ( vertexes[i+2].y * SM64_SCALE_FACTOR);
        surface_array[j].vertices[2][2] = (int16_t) (-vertexes[i+2].x * SM64_SCALE_FACTOR);

        j++;
    }

    sm64_static_surfaces_load(surface_array, j);

    ::free(surface_array);
}

int SM64Handler::mario_create(godot::Vector3 vec)
{
    if (!check_in_bounds(vec))
        return -2;

    int16_t x = (int16_t)(vec.z * SM64_SCALE_FACTOR);
    int16_t y = (int16_t)(vec.y * SM64_SCALE_FACTOR);
    int16_t z = (int16_t)(-vec.x * SM64_SCALE_FACTOR);
    return sm64_mario_create(x, y, z);
}

godot::Dictionary SM64Handler::mario_tick(int mario_id, godot::Dictionary inputs)
{
    godot::Dictionary ret;
    godot::Array mesh_array;
    struct SM64MarioState out_state;

    godot::Vector2 cam_look = inputs["cam_look"];
    godot::Vector2 stick = inputs["stick"];
    bool a = inputs["a"];
    bool b = inputs["b"];
    bool z = inputs["z"];

    struct SM64MarioInputs mario_inputs = {
        cam_look.y, // camLookX
        -cam_look.x, // camLookZ
        stick.x,
        stick.y,
        (uint8_t) a,
        (uint8_t) b,
        (uint8_t) z
    };

    sm64_mario_tick(mario_id, &mario_inputs, &out_state, &mario_geometry);

    ret["position"]   = godot::Vector3(-out_state.position[2] / SM64_SCALE_FACTOR, out_state.position[1] / SM64_SCALE_FACTOR, out_state.position[0] / SM64_SCALE_FACTOR);
    ret["velocity"]   = godot::Vector3(-out_state.velocity[2] / SM64_SCALE_FACTOR, out_state.velocity[1] / SM64_SCALE_FACTOR, out_state.velocity[0] / SM64_SCALE_FACTOR);
    ret["face_angle"] = (real_t) out_state.faceAngle;
    ret["health"]     = (int) out_state.health;

    mesh_array.resize(godot::ArrayMesh::ARRAY_MAX);
    mario_position.resize(mario_geometry.numTrianglesUsed*3);
    mario_normal.resize(mario_geometry.numTrianglesUsed*3);
    mario_color.resize(mario_geometry.numTrianglesUsed*3);
    mario_uv.resize(mario_geometry.numTrianglesUsed*3);

    invert_vertex_order(mario_geometry.position, mario_geometry.numTrianglesUsed);

    for (int i = 0; i < mario_geometry.numTrianglesUsed; i++)
    {
        godot::Vector3 positions[3];
        positions[0].z =  mario_geometry.position[9*i+0] / SM64_SCALE_FACTOR;
        positions[0].y =  mario_geometry.position[9*i+1] / SM64_SCALE_FACTOR;
        positions[0].x = -mario_geometry.position[9*i+2] / SM64_SCALE_FACTOR;
        positions[1].z =  mario_geometry.position[9*i+3] / SM64_SCALE_FACTOR;
        positions[1].y =  mario_geometry.position[9*i+4] / SM64_SCALE_FACTOR;
        positions[1].x = -mario_geometry.position[9*i+5] / SM64_SCALE_FACTOR;
        positions[2].z =  mario_geometry.position[9*i+6] / SM64_SCALE_FACTOR;
        positions[2].y =  mario_geometry.position[9*i+7] / SM64_SCALE_FACTOR;
        positions[2].x = -mario_geometry.position[9*i+8] / SM64_SCALE_FACTOR;
        mario_position.set(3*i+0, positions[0]);
        mario_position.set(3*i+1, positions[1]);
        mario_position.set(3*i+2, positions[2]);

        godot::Vector3 normals[3];
        normals[0].z =  mario_geometry.normal[9*i+0];
        normals[0].y =  mario_geometry.normal[9*i+1];
        normals[0].x = -mario_geometry.normal[9*i+2];
        normals[1].z =  mario_geometry.normal[9*i+3];
        normals[1].y =  mario_geometry.normal[9*i+4];
        normals[1].x = -mario_geometry.normal[9*i+5];
        normals[2].z =  mario_geometry.normal[9*i+6];
        normals[2].y =  mario_geometry.normal[9*i+7];
        normals[2].x = -mario_geometry.normal[9*i+8];
        mario_normal.set(3*i+0, normals[0]);
        mario_normal.set(3*i+1, normals[1]);
        mario_normal.set(3*i+2, normals[2]);

        mario_color.set(3*i+0, godot::Color(mario_geometry.color[9*i+0], mario_geometry.color[9*i+1], mario_geometry.color[9*i+2]));
        mario_color.set(3*i+1, godot::Color(mario_geometry.color[9*i+3], mario_geometry.color[9*i+4], mario_geometry.color[9*i+5]));
        mario_color.set(3*i+2, godot::Color(mario_geometry.color[9*i+6], mario_geometry.color[9*i+7], mario_geometry.color[9*i+8]));

        mario_uv.set(3*i+0, godot::Vector2(mario_geometry.uv[6*i+0], mario_geometry.uv[6*i+1]));
        mario_uv.set(3*i+1, godot::Vector2(mario_geometry.uv[6*i+2], mario_geometry.uv[6*i+3]));
        mario_uv.set(3*i+2, godot::Vector2(mario_geometry.uv[6*i+4], mario_geometry.uv[6*i+5]));
    }

    mesh_array[godot::ArrayMesh::ARRAY_VERTEX] = mario_position;
    mesh_array[godot::ArrayMesh::ARRAY_NORMAL] = mario_normal;
    mesh_array[godot::ArrayMesh::ARRAY_COLOR] = mario_color;
    mesh_array[godot::ArrayMesh::ARRAY_TEX_UV] = mario_uv;

    ret["mesh_array"] = mesh_array;

    return ret;
}

void SM64Handler::mario_delete(int mario_id)
{
    sm64_mario_delete(mario_id);
}

void SM64Handler::_register_methods()
{
    godot::register_method("global_init", &SM64Handler::global_init);
    godot::register_method("static_surfaces_load", &SM64Handler::static_surfaces_load);
    godot::register_method("mario_create", &SM64Handler::mario_create);
    godot::register_method("mario_tick", &SM64Handler::mario_tick);
    godot::register_method("mario_delete", &SM64Handler::mario_delete);
}
