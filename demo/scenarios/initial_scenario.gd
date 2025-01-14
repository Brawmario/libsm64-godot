extends Node3D


@export var start_cap := SM64Mario.Caps.NORMAL

@onready var mario := $SM64Mario as SM64Mario


func _ready() -> void:
	SM64Global.scale_factor = 75
	SM64Global.init()
	$SM64StaticSurfacesHandler.load_static_surfaces()
	$SM64SurfaceObjectsHandler.load_all_surface_objects()

	mario.create()
	mario.water_level = 0.0
	mario.interact_cap(start_cap)

	$HUD.mario = mario

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	
	$LibSM64AudioStreamPlayer.play()


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("toggle_mouse_lock"):
		_toggle_mouse_lock()


func _toggle_mouse_lock() -> void:
	if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)


func _on_tree_exiting() -> void:
	LibSM64.stop_background_music(LibSM64.get_current_background_music())
	mario.delete()
	SM64Global.terminate()
