#include <sm64_global.hpp>

#include <stdlib.h>
#include <string.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>

#include <audio/sm64_native_audio.hpp>
#include <audio/audio_wasapi.hpp>

static void SM64DebugPrintFunction(const char *msg)
{
    godot::UtilityFunctions::print(godot::String("[libsm64] ") + godot::String(msg) + godot::String("\n"));
}

SM64Global::SM64Global()
{
    ERR_FAIL_COND(singleton != nullptr);
    singleton = this;
}

SM64Global::~SM64Global()
{
    ERR_FAIL_COND(singleton != this);
    singleton = nullptr;

    terminate();
}

SM64Global *SM64Global::get_singleton()
{
    return singleton;
}

void SM64Global::init()
{
    ERR_FAIL_COND_MSG(m_rom_filepath.is_empty(), "[libsm64-godot] Called init without suppling ROM file.");
    ERR_FAIL_COND_MSG(!godot::FileAccess::file_exists(m_rom_filepath), "[libsm64-godot] ROM file not found.");

    const godot::String file_expected_sha256 = "17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91";
    const godot::String file_sha256 = godot::FileAccess::get_sha256(m_rom_filepath);

    ERR_FAIL_COND_MSG(file_sha256.is_empty(), "[libsm64-godot] Failed to get ROM file's SHA256.");
    ERR_FAIL_COND_MSG(file_sha256 != file_expected_sha256, "[libsm64-godot] ROM file doesnt have expected SHA256.");

    const godot::PackedByteArray rom = godot::FileAccess::get_file_as_bytes(m_rom_filepath);

    ERR_FAIL_COND_MSG(rom.is_empty(), "[libsm64-godot] Failed to read ROM file.");

    constexpr int64_t mario_texture_size = 4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT;
    uint8_t *mario_texture_raw = (uint8_t *) malloc(mario_texture_size);

#ifdef DEBUG_ENABLED
    sm64_register_debug_print_function(SM64DebugPrintFunction);
#endif // DEBUG_ENABLED

    sm64_global_init(rom.ptr(), mario_texture_raw);

    // sm64_audio_init leaks audio everytime it's called, make sure it's only called once
    static bool s_is_audio_init = false;
    if (unlikely(!s_is_audio_init))
    {
        sm64_audio_init(rom.ptr());
        s_is_audio_init = true;
    }

    // Audio registering (Windows only for now)
    #if HAVE_WASAPI
    if (sm64_native_audio_register(&g_audio_wasapi)) {
        godot::UtilityFunctions::print(godot::String("[libsm64-godot] Audio API: WASAPI"));
    }
    #endif // HAVE_WASAPI
    sm64_native_audio_start();

    godot::PackedByteArray mario_texture_packed;
    mario_texture_packed.resize(mario_texture_size);
    memcpy(mario_texture_packed.ptrw(), mario_texture_raw, mario_texture_size);

    m_mario_image = godot::Image::create_from_data(SM64_TEXTURE_WIDTH, SM64_TEXTURE_HEIGHT, false, godot::Image::FORMAT_RGBA8, mario_texture_packed);
    m_mario_image_texture = godot::ImageTexture::create_from_image(m_mario_image);

    m_init = true;

    ::free(mario_texture_raw);
}

void SM64Global::terminate()
{
    sm64_global_terminate();
    sm64_native_audio_stop();
    m_init = false;
}

bool SM64Global::is_init() const
{
    return m_init;
}

godot::Ref<godot::Image> SM64Global::get_mario_image()
{
    return m_mario_image;
}

godot::Ref<godot::ImageTexture> SM64Global::get_mario_image_texture()
{
    return m_mario_image_texture;
}

void SM64Global::set_rom_filepath(const godot::String &p_value)
{
    m_rom_filepath = p_value;
}

godot::String SM64Global::get_rom_filepath() const
{
    return m_rom_filepath;
}

void SM64Global::set_scale_factor(real_t p_value)
{
    m_scale_factor = p_value;
}

real_t SM64Global::get_scale_factor() const
{
    return m_scale_factor;
}

void SM64Global::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("init"), &SM64Global::init);
    godot::ClassDB::bind_method(godot::D_METHOD("terminate"), &SM64Global::terminate);
    godot::ClassDB::bind_method(godot::D_METHOD("is_init"), &SM64Global::is_init);
    godot::ClassDB::bind_method(godot::D_METHOD("get_mario_image"), &SM64Global::get_mario_image);
    godot::ClassDB::bind_method(godot::D_METHOD("get_mario_image_texture"), &SM64Global::get_mario_image_texture);
    godot::ClassDB::bind_method(godot::D_METHOD("set_rom_filepath", "value"), &SM64Global::set_rom_filepath);
    godot::ClassDB::bind_method(godot::D_METHOD("get_rom_filepath"), &SM64Global::get_rom_filepath);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::STRING, "rom_filepath", godot::PROPERTY_HINT_GLOBAL_FILE, "*.n64,*.z64,"), "set_rom_filepath", "get_rom_filepath");
    godot::ClassDB::bind_method(godot::D_METHOD("set_scale_factor", "value"), &SM64Global::set_scale_factor);
    godot::ClassDB::bind_method(godot::D_METHOD("get_scale_factor"), &SM64Global::get_scale_factor);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "scale_factor"), "set_scale_factor", "get_scale_factor");
}
