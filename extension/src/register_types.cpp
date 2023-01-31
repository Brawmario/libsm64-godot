#include <register_types.hpp>

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include <sm64_input.hpp>
#include <sm64_surface_properties.hpp>
#include <lib_sm64_gde.hpp>

using namespace godot;

static LibSM64 *_lib_sm64;

void initialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<SM64Input>();
    ClassDB::register_class<SM64SurfaceProperties>();
    ClassDB::register_class<LibSM64>();

    _lib_sm64 = memnew(LibSM64);
    Engine::get_singleton()->register_singleton("LibSM64", LibSM64::get_singleton());
}

void uninitialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    Engine::get_singleton()->unregister_singleton("LibSM64");
    memdelete(_lib_sm64);
}

extern "C" {
GDExtensionBool GDE_EXPORT libsm64gd_library_init(const GDExtensionInterface *p_interface, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

    init_obj.register_initializer(initialize_libsm64gd_module);
    init_obj.register_terminator(uninitialize_libsm64gd_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}