extends Node3D


@export var start_cap := SM64Mario.Caps.NORMAL

@onready var sm_64_mario = $SM64Mario as SM64Mario
@onready var sm_64_static_surface_handler = $SM64StaticSurfaceHandler as SM64StaticSurfaceHandler
@onready var hud = $HUD


func _ready() -> void:
	SM64Global.scale_factor = 175
	SM64Global.init()
	SM64Audio.play_music(SM64Audio.MUSIC_ID_LEVEL_GRASS)
	sm_64_static_surface_handler.load_static_surfaces()

	sm_64_mario.create()
	sm_64_mario.interact_cap(start_cap)
	sm_64_mario.face_angle = PI

	hud.mario = sm_64_mario

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("toggle_mouse_lock"):
		_toggle_mouse_lock()


func _toggle_mouse_lock() -> void:
	if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)


func _on_tree_exiting():
	SM64Audio.stop_background_music(SM64Audio.MUSIC_ID_LEVEL_GRASS)
	sm_64_mario.delete()
	SM64Global.terminate()
