extends Node3D


const BombOmbBattlefieldSurfaces = preload("res://libsm64_godot_demo/levels/bob_omb_battlefield/bob_omb_battlefield_surfaces.gd")

@export var start_cap := LibSM64.MarioFlags.MARIO_NORMAL_CAP

@onready var lib_sm_64_mario: LibSM64Mario = $LibSM64Mario

var _libsm64_was_init := false


func _ready() -> void:
	%BattlefieldMesh.mesh = BombOmbBattlefieldSurfaces.generate_godot_mesh()
	%BattlefieldMesh.mesh.surface_set_material(0, preload("res://libsm64_godot_demo/resources/bob_omb_battlefield_material.tres"))

	if LibSM64Global.rom.is_empty():
		%RomPickerDialog.pick_rom()
	else:
		_init_libsm64()


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed(&"ui_cancel"):
		_toggle_mouse_lock()


func _toggle_mouse_lock() -> void:
	if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	else:
		Input.mouse_mode = Input.MOUSE_MODE_CAPTURED


func _on_tree_exiting():
	if _libsm64_was_init:
		LibSM64.stop_background_music(LibSM64.SEQ_LEVEL_GRASS)
		lib_sm_64_mario.delete()
		LibSM64Global.terminate()


func _init_libsm64() -> void:
	_libsm64_was_init = LibSM64Global.init()
	if not _libsm64_was_init:
		push_error("Failed to initialize LibSM64Global")
		return

	BombOmbBattlefieldSurfaces.load_static_surfaces()

	lib_sm_64_mario.create()
	lib_sm_64_mario.interact_cap(start_cap)

	%HUD.mario = lib_sm_64_mario

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

	LibSM64.play_music(LibSM64.SEQ_PLAYER_LEVEL, LibSM64.SEQ_LEVEL_GRASS)
	%LibSM64AudioStreamPlayer.play()


func _on_rom_picker_dialog_rom_loaded() -> void:
	_init_libsm64()
