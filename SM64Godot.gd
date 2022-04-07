extends Node


export var rom_filename := "baserom.us.z64"

var sm64_handler


# Called when the node enters the scene tree for the first time.
func _ready():
	sm64_handler = load("res://SM64Handler.gdns").new()
	sm64_handler.global_init(rom_filename)


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func load_static_sufaces() -> void:
	var platform: MeshInstance = $"../Platform"
	var plataform_mesh := platform.get_mesh()
	var faces := plataform_mesh.get_faces()
	for i in range(faces.size()):
		faces[i] = platform.global_transform.xform(faces[i])
	sm64_handler.static_surfaces_load(faces)
