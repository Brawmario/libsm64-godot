extends FileDialog


signal rom_loaded


func _on_file_selected(path: String) -> void:
	if LibSM64Global.load_rom_file(path):
		rom_loaded.emit()
	else:
		%InvalidRomDialog.popup_centered()
