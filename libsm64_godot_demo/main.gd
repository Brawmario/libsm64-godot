extends Control


func _ready() -> void:
	if LibSM64Global.rom.is_empty():
		%RomPickerDialog.pick_rom()


func return_to_menu() -> void:
	%LevelContainer.get_child(0).queue_free()
	show()


func load_level(scene: PackedScene) -> void:
	hide()
	%LevelContainer.add_child(scene.instantiate())


func _on_pick_rom_button_pressed() -> void:
	%RomPickerDialog.pick_rom()


func _on_initial_scenario_button_pressed() -> void:
	load_level(preload("res://libsm64_godot_demo/demo_scenes/initial_scenario/initial_scenario.tscn"))


func _on_many_marios_button_pressed():
	load_level(preload("res://libsm64_godot_demo/demo_scenes/many_marios/many_marios.tscn"))


func _on_flying_range_button_pressed() -> void:
	load_level(preload("res://libsm64_godot_demo/demo_scenes/flying_range/flying_range.tscn"))


func _on_bob_omb_battlefield_button_pressed():
	load_level(preload("res://libsm64_godot_demo/demo_scenes/bob_omb_battlefield/bob_omb_battlefield.tscn"))
