# libsm64-godot

Addon that binds the libsm64 to godot via GDExtention, allowing to integrate mario into any Godot 4 project.

![](https://i.imgur.com/ilo5orj.png)

## Current godot compatability

Currently compatible with Godot 4.0 beta 1, with binaries compiled for Windows and Linux 64 bits. Check the releases for the files.

## How to

This addon adds two main components:

- *SM64Handler*: Extends Resource. Should have one instance in your project's filesystem.
- *Mario3D*: Extends Node3D. Can have multiple in any scene.

It also adds two helper nodes:

- *StaticSurfaceHandler*: Extends Node. provides c to load all the meshes under the `libsm64_static_surfaces` group.
- *SurfaceObjectsHandler*: Extends Node. provides helper functions for adding moving objects the libsm64 world.

Simple steps for basic use:

1. Create one instance of *SM64Handler* in the filesystem.
2. Feed the *SM64Handler* the filepath to the Super Mario 64 ROM file (Search for one with your favorite search engine).
3. Call the `SM64Handler.global_init()` function on the instance.
4. Add the meshes that compose the world to the `libsm64_static_surfaces` group (NOTICE: use simple meshes for best results!!!).
5. Add a *StaticSurfaceHandler* node to the scene and call `StaticSurfaceHandler.load_static_sufaces()`.
6. Add a *Mario3D* (or as many as you want) and call `Mario3D.create()` (Set the actions in the "Input Actions" export group in order to control Mario).
