#include <SM64Handler.hpp>

#include <string>
#include <cstdio>

extern "C"
{
#include <libsm64.h>
}

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

SM64Handler::SM64Handler()
{
}

void SM64Handler::_init()
{
}

SM64Handler::~SM64Handler()
{
    sm64_global_terminate();

    if (mario_texture)
        ::free(mario_texture);
}

void SM64Handler::global_init(godot::String rom_filename)
{
    size_t rom_size;
    uint8_t *rom = utils_read_file_alloc(rom_filename.ascii().get_data(), &rom_size);

    if (rom == NULL)
    {
        godot::Godot::print(godot::String("[SM64Handler] Failed to read ROM file: ") + rom_filename);
        return;
    }

    mario_texture = (uint8_t *)malloc(4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT);

    sm64_global_init(rom, mario_texture, SM64DebugPrintFunction);

    ::free(rom);
}

void SM64Handler::static_surfaces_load(godot::PoolVector3Array vertexes)
{
    uint32_t num_surfaces = vertexes.size() / 3;
    struct SM64Surface *surface_array = (SM64Surface *)malloc(sizeof(SM64Surface) * num_surfaces);

    uint32_t j = 0;
    for (size_t i = 0; i < vertexes.size(); i += 3)
    {
        if (!check_in_bounds(vertexes[i]) || !check_in_bounds(vertexes[i+1]) || !check_in_bounds(vertexes[i+2]))
        {
            num_surfaces--;
            continue;
        }

        surface_array[j].type = 0x0000; // SURFACE_DEFAULT
        surface_array[j].force = 0;
        surface_array[j].terrain = 0x0002; // TERRAIN_SNOW
        surface_array[j].vertices[0][0] = (int16_t) (vertexes[i].z * SM64_SCALE_FACTOR);
        surface_array[j].vertices[0][1] = (int16_t) (vertexes[i].y * SM64_SCALE_FACTOR);
        surface_array[j].vertices[0][2] = (int16_t) (-vertexes[i].x * SM64_SCALE_FACTOR);
        surface_array[j].vertices[1][0] = (int16_t) (vertexes[i+1].z * SM64_SCALE_FACTOR);
        surface_array[j].vertices[1][1] = (int16_t) (vertexes[i+1].y * SM64_SCALE_FACTOR);
        surface_array[j].vertices[1][2] = (int16_t) (-vertexes[i+1].x * SM64_SCALE_FACTOR);
        surface_array[j].vertices[2][0] = (int16_t) (vertexes[i+2].z * SM64_SCALE_FACTOR);
        surface_array[j].vertices[2][1] = (int16_t) (vertexes[i+2].y * SM64_SCALE_FACTOR);
        surface_array[j].vertices[2][2] = (int16_t) (-vertexes[i+2].x * SM64_SCALE_FACTOR);

        j++;
    }

    sm64_static_surfaces_load(surface_array, num_surfaces);

    ::free(surface_array);
}

void SM64Handler::_register_methods()
{
    godot::register_method("global_init", &SM64Handler::global_init);
    godot::register_method("static_surfaces_load", &SM64Handler::static_surfaces_load);
}
