extends Node3D


const BombOmbMinimalSurfaces = preload("./bob_omb_minimal_surfaces.gd")

@export var start_cap := LibSM64.MarioFlags.MARIO_NORMAL_CAP

@onready var battlefield: MeshInstance3D = $Battlefield
@onready var sm_64_mario: Node3D = $LibSM64Mario


func _ready() -> void:
	if not LibSM64Global.init():
		push_error("Failed to initialize LibSM64Global")
		return

	battlefield.mesh = BombOmbMinimalSurfaces.generate_godot_mesh()
	battlefield.mesh.surface_set_material(0, preload("./bob_omb_minimal_material.tres"))
	BombOmbMinimalSurfaces.load_static_surfaces()

	sm_64_mario.create()
	sm_64_mario.interact_cap(start_cap)

	$HUD.mario = sm_64_mario

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

	LibSM64.play_music(LibSM64.SEQ_PLAYER_LEVEL, LibSM64.SEQ_LEVEL_GRASS)
	$LibSM64AudioStreamPlayer.play()


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("toggle_mouse_lock"):
		_toggle_mouse_lock()


func _toggle_mouse_lock() -> void:
	if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	else:
		Input.mouse_mode = Input.MOUSE_MODE_CAPTURED


func _on_tree_exiting():
	LibSM64.stop_background_music(LibSM64.SEQ_LEVEL_GRASS)
	sm_64_mario.delete()
	LibSM64Global.terminate()
