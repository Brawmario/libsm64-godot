extends Node3D


@export var start_cap := LibSM64.MarioFlags.MARIO_NORMAL_CAP

var _libsm64_was_init := false


func _ready() -> void:
	if LibSM64Global.rom.is_empty():
		%RomPickerDialog.popup_centered_ratio()
	else:
		_init_libsm64()


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed(&"ui_cancel"):
		_toggle_mouse_lock()


func _toggle_mouse_lock() -> void:
	if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)


func _on_tree_exiting() -> void:
	if _libsm64_was_init:
		LibSM64.stop_background_music(LibSM64.get_current_background_music())
		%LibSM64Mario.delete()
		LibSM64Global.terminate()


func _init_libsm64() -> void:
	_libsm64_was_init = LibSM64Global.init()
	if not _libsm64_was_init:
		push_error("Failed to initialize LibSM64Global")
		return

	%LibSM64StaticSurfacesHandler.load_static_surfaces()
	%LibSM64SurfaceObjectsHandler.load_all_surface_objects()

	%LibSM64Mario.create()
	%LibSM64Mario.water_level = 0.0
	%LibSM64Mario.interact_cap(start_cap)

	%HUD.mario = %LibSM64Mario

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

	%LibSM64AudioStreamPlayer.play()


func _on_rom_picker_dialog_rom_loaded() -> void:
	_init_libsm64()
