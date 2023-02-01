# libsm64-godot

Addon that binds the libsm64 to godot via GDExtention, allowing to integrate mario into any Godot 4 project.

![](https://i.imgur.com/l8qOnad.png)

## Current Godot Compatibility

Currently compatible with Godot 4.0 beta 16, with binaries compiled for Windows and Linux 64 bits. Check the [https://github.com/Brawmario/libsm64-godot/releases](releases) for the files.

## How to

This add-on consists of two main components:

- *SM64*: Singleton that exposes the funcitions of the libsm64 library
- *SM64Mario*: Extends Node3D; can have multiple in any scene.

It also adds two helper nodes:

- *StaticSurfaceHandler*: Extends Node; provides helper functions to load all the meshes under the `libsm64_static_surfaces` group.
- *SurfaceObjectsHandler*: Extends Node; provides helper functions to load and update all the meshes under the `libsm64_surface_objects` group.

This add-on requires a Super Mario 64 (USA) ROM file (for legal reasons, steps to get a ROM will not be disclosed). For the included demo project, make sure the ROM file has the following SHA256 hash:

>17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91

## Simple steps for basic use

When creating the scene:

1. Add a *SM64StaticSurfaceHandler* node to the scene
2. Add the meshes that compose the world to the `libsm64_static_surfaces` group (NOTICE: use simple, low-polycount meshes for best results).
3. Add a *SM64Mario* node (or as many as you want) and set its camera (Set the actions in the "Input Actions" export group in order to control Mario).

Then do the following on the scene's main script:

1. Feed the *SM64* singleton the file path to the Super Mario 64 ROM file (`SM64.rom_filepath = <path-to-rom>`).
2. Call the `SM64.global_init()` function.
3. Call `StaticSurfaceHandler.load_static_surfaces()` on your *SM64StaticSurfaceHandler* node.
4. Call `SM64Mario.create()` on your *SM64Mario* node(s).
