#include <register_types.hpp>

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include <sm64_global.hpp>
#include <sm64_surfaces.hpp>
//#include <sm64_audio.hpp>
#include <sm64_mario_internal.hpp>
#include <sm64_input.hpp>
#include <sm64_surface_properties.hpp>
#include <sm64_audio_stream_player.hpp>

#include <libsm64.hpp>
#include <libsm64_surface_properties.hpp>
#include <libsm64_mario_inputs.hpp>
#include <libsm64_mario_state.hpp>
#include <libsm64_mario_tick_output.hpp>
#include <libsm64_surface_array.hpp>
#include <libsm64_mario_internal.hpp>
#include <libsm64_audio_stream_player.hpp>

using namespace godot;

static SM64Global *s_sm64_global;
static SM64Surfaces *s_sm64_surfaces;
//static SM64Audio *s_sm64_audio;
static LibSM64 *s_libsm64;

void initialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<SM64Global>();
    ClassDB::register_class<SM64Surfaces>();
    //ClassDB::register_class<SM64Audio>();
    ClassDB::register_class<SM64MarioInternal>();
    ClassDB::register_class<SM64Input>();
    ClassDB::register_class<SM64SurfaceProperties>();
    ClassDB::register_class<SM64AudioStreamPlayer>();

    ClassDB::register_class<LibSM64>();
    ClassDB::register_class<LibSM64SurfaceProperties>();
    ClassDB::register_class<LibSM64MarioInputs>();
    ClassDB::register_class<LibSM64MarioState>();
    ClassDB::register_class<LibSM64MarioTickOutput>();
    ClassDB::register_class<LibSM64SurfaceArray>();
    ClassDB::register_class<LibSM64MarioInternal>();
    ClassDB::register_class<LibSM64AudioStreamPlayer>();

    s_sm64_global = memnew(SM64Global);
    Engine::get_singleton()->register_singleton("SM64Global", SM64Global::get_singleton());

    s_sm64_surfaces = memnew(SM64Surfaces);
    Engine::get_singleton()->register_singleton("SM64Surfaces", SM64Surfaces::get_singleton());

    //s_sm64_audio = memnew(SM64Audio);
    //Engine::get_singleton()->register_singleton("SM64Audio", SM64Audio::get_singleton());

    s_libsm64 = memnew(LibSM64);
    Engine::get_singleton()->register_singleton("LibSM64", LibSM64::get_singleton());
}

void uninitialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    Engine::get_singleton()->unregister_singleton("SM64Global");
    memdelete(s_sm64_global);

    Engine::get_singleton()->unregister_singleton("SM64Surfaces");
    memdelete(s_sm64_surfaces);

    //Engine::get_singleton()->unregister_singleton("SM64Audio");
    //memdelete(s_sm64_audio);

    Engine::get_singleton()->unregister_singleton("LibSM64");
    memdelete(s_libsm64);
}

extern "C" {
GDExtensionBool GDE_EXPORT libsm64gd_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_libsm64gd_module);
    init_obj.register_terminator(uninitialize_libsm64gd_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
