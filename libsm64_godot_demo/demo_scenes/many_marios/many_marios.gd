extends Node3D


var _libsm64_was_init := false


func _ready() -> void:
	if LibSM64Global.rom.is_empty():
		%RomPickerDialog.pick_rom()
	else:
		_init_libsm64()


func _on_tree_exiting() -> void:
	if _libsm64_was_init:
		LibSM64.stop_background_music(LibSM64.get_current_background_music())
		for node in get_tree().get_nodes_in_group("libsm64_mario"):
			var libsm64_mario := node as LibSM64Mario
			if not libsm64_mario:
				continue
			libsm64_mario.delete()
		LibSM64Global.terminate()


func _init_libsm64() -> void:
	_libsm64_was_init = LibSM64Global.init()
	if not _libsm64_was_init:
		push_error("Failed to initialize LibSM64Global")
		return

	%LibSM64StaticSurfacesHandler.load_static_surfaces()
	%LibSM64SurfaceObjectsHandler.load_all_surface_objects()

	for node in get_tree().get_nodes_in_group("libsm64_mario"):
		var libsm64_mario := node as LibSM64Mario
		if not libsm64_mario:
			continue
		libsm64_mario.create()
		libsm64_mario.water_level = 0.0

	%LibSM64AudioStreamPlayer.play()


func _on_rom_picker_dialog_rom_loaded() -> void:
	_init_libsm64()
