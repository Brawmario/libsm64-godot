extends Node3D


@export var start_cap := LibSM64.MarioFlags.MARIO_NORMAL_CAP

@onready var mario := $LibSM64Mario as LibSM64Mario


func _ready() -> void:
	if not LibSM64Global.init():
		push_error("Failed to initialize LibSM64Global")
		return

	$LibSM64StaticSurfacesHandler.load_static_surfaces()
	$LibSM64SurfaceObjectsHandler.load_all_surface_objects()

	mario.create()
	mario.water_level = 0.0
	mario.interact_cap(start_cap)

	$HUD.mario = mario

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

	$LibSM64AudioStreamPlayer.play()


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed(&"ui_cancel"):
		_toggle_mouse_lock()


func _toggle_mouse_lock() -> void:
	if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)


func _on_tree_exiting() -> void:
	LibSM64.stop_background_music(LibSM64.get_current_background_music())
	mario.delete()
	LibSM64Global.terminate()
