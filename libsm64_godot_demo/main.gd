extends Control


@onready var rom_picker_dialog := $RomPickerDialog as FileDialog
@onready var invalid_rom_dialog : = $InvalidRomDialog as AcceptDialog


func _ready() -> void:
	if LibSM64Global.rom.is_empty():
		rom_picker_dialog.popup_centered_ratio()


func return_to_menu() -> void:
	$LevelContainer.get_child(0).queue_free()
	show()


func load_level(scene: PackedScene) -> void:
	$LevelContainer.add_child(scene.instantiate())
	hide()


func _on_rom_picker_dialog_file_selected(path: String) -> void:
	if not LibSM64Global.load_rom_file(path):
		invalid_rom_dialog.popup_centered()


func _on_invalid_rom_dialog_confirmed() -> void:
	rom_picker_dialog.popup_centered_ratio()


func _on_pick_rom_button_pressed() -> void:
	rom_picker_dialog.popup_centered_ratio()


func _on_initial_scenario_button_pressed() -> void:
	load_level(preload("./scenarios/initial_scenario.tscn"))


func _on_many_marios_button_pressed():
	load_level(preload("./scenarios/many_marios.tscn"))


func _on_flying_range_button_pressed() -> void:
	load_level(preload("./scenarios/flying_range.tscn"))


func _on_bob_omb_battlefield_button_pressed():
	load_level(preload("./scenarios/bob_omb_minimal/bob_omb_minimal.tscn"))
