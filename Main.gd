extends Spatial


export var sm64_handler: Resource

onready var rom_picker: FileDialog = $FileDialog


func _ready():
	rom_picker.popup_centered()
	yield(rom_picker, "file_selected")
	sm64_handler.rom_filename = rom_picker.current_path
	
	sm64_handler.global_init()
	load_static_sufaces()
	for mario in get_tree().get_nodes_in_group("Mario"):
		mario.create()


func load_static_sufaces() -> void:
	var platform: MeshInstance = $Platform
	var wall: MeshInstance = $Wall
	var platform_faces := platform.get_mesh().get_faces()
	var wall_faces = wall.get_mesh().get_faces()
	for i in range(wall_faces.size()):
		wall_faces[i] += wall.global_transform.origin
	for i in range(platform_faces.size()):
		platform_faces[i] += platform.global_transform.origin
	platform_faces.append_array(wall_faces)
	sm64_handler.static_surfaces_load(platform_faces)
