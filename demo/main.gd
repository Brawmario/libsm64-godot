extends Control


const ROM_SHA256 := "17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91"

@export var sm64_handler: SM64Handler

@onready var rom_picker_dialog := $FileDialog as FileDialog
@onready var invalid_rom_dialog : = $AcceptDialog as AcceptDialog


func _ready() -> void:
	if sm64_handler.rom_filename == "":
		var rom_file_path := ""
		while rom_file_path == "":
			rom_file_path = await _select_rom_file()
		sm64_handler.rom_filename = rom_file_path


func _select_rom_file() -> String:
	rom_picker_dialog.popup_centered_ratio()
	await rom_picker_dialog.file_selected

	var rom_file_path := rom_picker_dialog.current_path
	var rom_file_sha256 := FileAccess.get_sha256(rom_file_path)

	if rom_file_sha256 != ROM_SHA256:
		invalid_rom_dialog.popup_centered()
		await invalid_rom_dialog.confirmed
		return ""

	return rom_file_path


func _on_initial_scenario_button_pressed() -> void:
	get_tree().change_scene_to_packed(preload("res://demo/scenarios/initial_scenario.tscn"))
