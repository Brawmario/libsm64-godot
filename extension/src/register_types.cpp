#include <register_types.hpp>

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include <libsm64.hpp>
#include <libsm64_surface_properties.hpp>
#include <libsm64_mario_inputs.hpp>
#include <libsm64_mario_state.hpp>
#include <libsm64_mario_tick_output.hpp>
#include <libsm64_surface_array.hpp>
#include <libsm64_mario_interpolator.hpp>
#include <libsm64_audio_stream_player.hpp>

using namespace godot;

static LibSM64 *s_libsm64;

void initialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<LibSM64>();
    ClassDB::register_class<LibSM64SurfaceProperties>();
    ClassDB::register_class<LibSM64MarioInputs>();
    ClassDB::register_class<LibSM64MarioState>();
    ClassDB::register_class<LibSM64MarioTickOutput>();
    ClassDB::register_class<LibSM64SurfaceArray>();
    ClassDB::register_class<LibSM64MarioInterpolator>();
    ClassDB::register_class<LibSM64AudioStreamPlayer>();

    s_libsm64 = memnew(LibSM64);
    Engine::get_singleton()->register_singleton("LibSM64", LibSM64::get_singleton());
}

void uninitialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

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
