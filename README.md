# libsm64-godot

Addon that binds the libsm64 to godot via GDExtention, allowing to integrate mario into any Godot 4 project.

![](https://i.imgur.com/ilo5orj.png)

## Current Godot Compatibility

Currently compatible with Godot 4.0 beta 1 - 3, with binaries compiled for Windows and Linux 64 bits. Check the [https://github.com/Brawmario/libsm64-godot/releases](releases) for the files.

## How to

This add-on consists of two main components:

- *SM64Mario*: Extends Node3D; can have multiple in any scene.
- *SM64Handler*: Extends Resource; should have one instance in your project's filesystem, but used across one or several nodes when necessary.

It also adds two helper nodes:

- *StaticSurfaceHandler*: Extends Node; provides C to load all the meshes under the `libsm64_static_surfaces` group.
- *SurfaceObjectsHandler*: Extends Node; provides helper functions for adding moving objects the libsm64 world.

Simple steps for basic use:

1. Add one instance of a *SM64Mario* into the scene tree.
2. Feed *SM64Mario.sm64_handler* the filepath to the Super Mario 64 ROM file (for legal reasons, steps to get a ROM will not be disclosed).
3. Call the `SM64Mario.sm64_handler.global_init()` function.
4. Add the meshes that compose the world to the `libsm64_static_surfaces` group (NOTICE: use simple, low-polycount meshes for best results).
5. Add a *SM64StaticSurfaceHandler* node to the scene and call `StaticSurfaceHandler.load_static_surfaces()`.
6. Add a *SM64Mario* (or as many as you want) and call `Mario3D.create()` (Set the actions in the "Input Actions" export group in order to control Mario).
