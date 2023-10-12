# LibSM64 Godot addon manual

## Installation

Grab the latest [release](https://github.com/Brawmario/libsm64-godot/releases) (the `libsm64-godot-addon.vX.Y.Z.zip` file). Unzip the `libsm64-godot` into a `addons` folder on the root of your project folder.

If done correctly, the addon should show up under `Project Settings > Plugins`. Enable the addon on this same screen.

[Enable the LibSM64 Godot addon](enable-addon.png)

Note: You might get some weird `Error loading extension: res://addons/libsm64-godot/extension/libsm64.gd.gdextension` message. Try relaoding the project or re-adding the addon, this error seems inconsistant.

## How to setup basic use

### Providing the ROM

In order to use libsm64-godot, a ROM of Super Mario 64 (USA) is necessary (for legal reasons, steps to get a ROM will not be disclosed). Make sure the ROM file has the following SHA256 hash:

>17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91

The path to this ROM file needs to be provided to the `SM64Global` singleton on the `rom_filepath` property before you can call the `SM64Global.init()` method.

The ROM file does not need to be included in a project that uses this addon and can be dynamically sourced from the user at runtime in order to avoid distribuing copyrighted material. Check the `main` scene in the included demo project on this repository for an example on how to request a filepath from the user at runtime and then use it to fill the `SM64Global.rom_filepath` property.

### Setting up a scene

For this example I will setup a basic 3D scene with meshses. If you are trying to put Mario in a already existing scene you can reuse the already existing meshes, but it is best to use the simplest meshses possible for collision (you can make the meshes invisible if you're only creating them to establish collision).

The scene for this example is this:

[Basic scene](basic-scene.png)

Mesh initalizing

Static Surfaces

SM64MArio Node setup

## Futher setup

Surface Objects

Surface properties

## Quirks

Have a lower plane

Low poly mesh (make collion specific meshes)
