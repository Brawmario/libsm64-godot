#include <register_types.hpp>

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include <libsm64gde.hpp>
#include <sm64_input.hpp>
#include <sm64_surface_properties.hpp>

using namespace godot;

static SM64 *_sm64;

void initialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<SM64>();
    ClassDB::register_class<SM64Input>();
    ClassDB::register_class<SM64SurfaceProperties>();

    _sm64 = memnew(SM64);
    Engine::get_singleton()->register_singleton("SM64", SM64::get_singleton());
}

void uninitialize_libsm64gd_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    Engine::get_singleton()->unregister_singleton("SM64");
    memdelete(_sm64);
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