extends Control


var _file_access_web: FileAccessWeb


func _ready() -> void:
	if LibSM64Global.rom.is_empty():
		_ask_for_rom_file()


func return_to_menu() -> void:
	%LevelContainer.get_child(0).queue_free()
	show()


func load_level(scene: PackedScene) -> void:
	%LevelContainer.add_child(scene.instantiate())
	hide()


func _ask_for_rom_file() -> void:
	if OS.get_name() == "Web":
		_file_access_web = FileAccessWeb.new()
		_file_access_web.loaded.connect(_on_file_access_web_loaded)
		_file_access_web.open(".z64")
	else:
		%RomPickerDialog.popup_centered_ratio()


func _on_file_access_web_loaded(_file_name: String, _type: String, base64_data: String) -> void:
	LibSM64Global.rom = Marshalls.base64_to_raw(base64_data)


func _on_pick_rom_button_pressed() -> void:
	_ask_for_rom_file()


func _on_initial_scenario_button_pressed() -> void:
	load_level(preload("res://libsm64_godot_demo/demo_scenes/initial_scenario/initial_scenario.tscn"))


func _on_many_marios_button_pressed():
	load_level(preload("res://libsm64_godot_demo/demo_scenes/many_marios/many_marios.tscn"))


func _on_flying_range_button_pressed() -> void:
	load_level(preload("res://libsm64_godot_demo/demo_scenes/flying_range/flying_range.tscn"))


func _on_bob_omb_battlefield_button_pressed():
	load_level(preload("res://libsm64_godot_demo/demo_scenes/bob_omb_battlefield/bob_omb_battlefield.tscn"))
