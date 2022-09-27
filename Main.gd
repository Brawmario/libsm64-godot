extends Node3D

@export var sm64_handler: Resource

@onready var rom_picker: FileDialog = $FileDialog


func _ready() -> void:
	rom_picker.popup_centered_ratio()
	await rom_picker.file_selected
	sm64_handler.rom_filename = rom_picker.current_path

	sm64_handler.global_init()
	$StaticSurfacesHandler.load_static_sufaces()
	$SurfaceObjectsHandler.load_all_surface_objects()

	for mario in get_tree().get_nodes_in_group("Mario"):
		mario.create()

	$Objects/MovingPlatform/AnimationPlayer.play("Move")
