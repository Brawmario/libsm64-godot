extends Node3D

@export var sm64_handler: SM64Handler

@onready var rom_picker: FileDialog = $FileDialog


func _ready() -> void:
	rom_picker.popup_centered_ratio()
	await rom_picker.file_selected
	sm64_handler.rom_filename = rom_picker.current_path

	sm64_handler.global_init()
	$SM64StaticSurfacesHandler.load_static_sufaces()
	$SM64SurfaceObjectsHandler.load_all_surface_objects()

	for node in get_tree().get_nodes_in_group("libsm64_mario"):
		var mario := node as SM64Mario
		if not mario:
			continue
		mario.create()
		mario.water_level = 0.0

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
