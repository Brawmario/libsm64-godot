# LibSM64 Godot addon manual

## Installation

Grab the latest [release](https://github.com/Brawmario/libsm64-godot/releases) (the `libsm64-godot-addon.vX.Y.Z.zip` file). Unzip the `libsm64-godot` into a `addons` folder on the root of your project folder.

If done correctly, the addon should show up under `Project Settings > Plugins`. Enable the addon on this same screen.

![Enable the LibSM64 Godot addon](enable-addon.png)

Note: You might get a `Error loading extension: res://addons/libsm64-godot/extension/libsm64.gd.gdextension` message. Try relaoding the project or re-adding the addon, this error seems inconsistent.

## How to setup basic use

### Providing the ROM

In order to use libsm64-godot, a ROM of Super Mario 64 (USA) is necessary (for legal reasons, steps to get a ROM will not be disclosed). Make sure the ROM file has the following SHA256 hash:

>17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91

The path to this ROM file needs to be provided to the `SM64Global` singleton on the `rom_filepath` property before you can call the `SM64Global.init()` method. If the SHA256 doesn't match, it will be rejected and the addon will fail to initialize.

The ROM file does not need to be included with a project that uses this addon and can be dynamically sourced from the user at runtime in order to avoid distribuing copyrighted material. Check the `main` scene in the included demo project on this repository for an example on how to request a filepath from the user at runtime and then use it to fill the `SM64Global.rom_filepath` property.

### Setting up a scene

For this example I will setup a basic 3D scene with meshses that will serve as static collision surfaces. If you are trying to put Mario in a already existing scene you can reuse the already existing meshes, but it is best to use the simplest meshses possible for collision (you can make the meshes invisible if you're only creating them to establish collision).

The scene for this example is this:

![Basic scene](basic-scene.png)

The floor a is 20 meters by 20 meters plane and the pillar is a 5 meters tall rectangular prism.

### Static Surfaces

In order to allow the addon to use the meshes as collision, you'll need to add an `SM64StaticSurfaceHandler` node to the scene via the Create New Node dialog. Then, you'll need to add all the meshes that will serve as the Static Surfaces of the world to the node group specified by the `static_surfaces_group` field of the `SM64StaticSurfaceHandler` node (by default the group name will be `libsm64_static_surfaces`, you shouldn't really need to change this).

![Create SM64StaticSurfaceHandler node](sm64staticsurfacehandler-add.png)

![Static surface node setup](static-surface-node-setup.png)

### SM64Mario node

Create a `SM64Mario` node and add it to the scene. You'll also want to add a `Camera3D` node to the scene at this point. The camera will be static for this demo.

![Create SM64Mario node](sm64mario-node-add.png)

Position the camera appropriately and position the `SM64Mario` node a little bit above the plane. Inspect the `SM64Mario` node, click on the `Camera` property and pick the `Camera3D` in the scene tree.

![Scene Tree after adding the SM64Mario node](sm64mario-scene.png)

Take note of the properties under the `Input Actions` export group. These are the action names used by the node in order to control Mario. Either change these action names to other names already in the project's Input Map (such as the `ui_*` action names) or create these action names in the project's Input Map and bind them to the appropriate axes/buttons/keys.

![Example Input Map](example-input-map.png)

### Final scene setup - basic script

Add a new script on the root node. The following script snippet is a simple example on how to initialize the `libsm64-godot` world and how to initialize the `SM64Mario` node.

```Swift
extends Node3D


@onready var sm_64_mario: SM64Mario = $SM64Mario
@onready var sm_64_static_surface_handler: Node = $SM64StaticSurfaceHandler


func _ready() -> void:
	# Supply the path to the ROM file.
	# Avoid hardcoding this parameter, you should get this path on runtime.
	SM64Global.rom_filepath = "/path/to/rom/file"
	# Set the scale of the internal `libsm64` world. The bigger the scale, the smaller Mario will be in the Godot scene.
	# At 75.0 scale, Mario will be just below 2 meters tall in the Godot scene.
	SM64Global.scale_factor = 75.0

	# Init the `libsm64` world.
	SM64Global.init()

	# Init the static surfaces (make sure the relevant MeshIntance3D nodes are ready and in the appropriate group).
	sm_64_static_surface_handler.load_static_surfaces()

	# Initialize the SM64Mario node.
	# both the `libsm64` world and the Static Surfaces must be already initialized without errors.
	sm_64_mario.create()


func _on_tree_exiting() -> void:
	# Clean up the `libsm64` world when the scene is freed.
	sm_64_mario.delete()
	SM64Global.terminate()
```

If everything goes correctly, you should be able to run this scene and get it all to work.

![Final running scene](running-scene.png)

## Futher setup

### Surface Objects

TODO

### Surface properties

TODO

## Quirks

### Have a big plane below your world added to the Static Surfaces

TODO

### Low poly mesh (make collion specific meshes)

TODO