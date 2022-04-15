# libsm64-godot

Addon that binds the libsm64 to godot via GDNative, allowing to integrate mario into any Godot project

![](https://i.imgur.com/ilo5orj.png)

## How to

Pull this repo and initialize the submodules.

compile the `godot-cpp` and `libsm64` projects individually.

run the compilation script in the `addons/libsm64/sm64_handler` folder for the
desired platform (Windows requires a MinGW instalation).

This addon adds two componets:

- *SM64Handler*: Resource. Should have one instance in your project's filesystem.
- *Mario*: Spatial node. Can have multiple in any scene.

You should first call `global_init()` on the SM64Handler after setting the
ROM's path on the instance. After that, feed the faces of the statics surfaces'
meshes to the `static_surfaces_load()` (better interface pending). Lastly, Call
`create()` on the Mario nodes to initialize them. This repo works as an example
project, check it out!