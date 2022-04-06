#include <SM64Handler.hpp>

#include <string>
#include <cstdio>

extern "C"
{
#include <libsm64.h>
}

static void SM64DebugPrintFunction(const char *msg)
{
    godot::Godot::print(godot::String("[libsm64] ") + godot::String(msg));
}

static uint8_t* utils_read_file_alloc(const char *path, size_t *fileLength)
{
    FILE *f = fopen(path, "rb");

    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    size_t length = (size_t) ftell(f);
    rewind(f);
    uint8_t *buffer = (uint8_t *) malloc(length + 1);
    fread(buffer, 1, length, f);
    buffer[length] = 0;
    fclose(f);

    if (fileLength)
        *fileLength = length;

    return buffer;
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

void SM64Handler::_register_methods()
{
    godot::register_method("global_init", &SM64Handler::global_init);
}
