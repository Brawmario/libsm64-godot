extends Control


const ROM_SHA256 := "17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91"

@onready var rom_picker_dialog := $RomPickerDialog as FileDialog
@onready var invalid_rom_dialog : = $InvalidRomDialog as AcceptDialog


func _ready() -> void:
	if SM64Global.rom_filepath == "":
		rom_picker_dialog.popup_centered_ratio()


func _on_rom_picker_dialog_file_selected(path: String) -> void:
	var rom_file_sha256 := FileAccess.get_sha256(path)

	if rom_file_sha256 != ROM_SHA256:
		invalid_rom_dialog.popup_centered()
		return

	SM64Global.rom_filepath = path


func _on_invalid_rom_dialog_confirmed() -> void:
	rom_picker_dialog.popup_centered_ratio()


func _on_pick_rom_button_pressed() -> void:
	rom_picker_dialog.popup_centered_ratio()


func _on_initial_scenario_button_pressed() -> void:
	get_tree().change_scene_to_packed(preload("res://demo/scenarios/initial_scenario.tscn"))


func _on_many_marios_button_pressed():
	get_tree().change_scene_to_packed(preload("res://demo/scenarios/many_marios.tscn"))


func _on_flying_range_button_pressed() -> void:
	get_tree().change_scene_to_packed(preload("res://demo/scenarios/flying_range.tscn"))


func _on_bob_omb_battlefield_button_pressed():
	get_tree().change_scene_to_packed(preload("res://demo/scenarios/bob-omb_minimal/bob-omb_minimal.tscn"))
