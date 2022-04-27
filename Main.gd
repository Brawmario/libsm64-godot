extends Spatial


const FPS_30_DELTA = 1.0/30.0

export var sm64_handler: Resource

var time_since_last_tick := 0.0
var moving_platform_id := -1

onready var rom_picker: FileDialog = $FileDialog
onready var moving_platform: MeshInstance = $Objects/MovingPlatform


func _ready() -> void:
	rom_picker.popup_centered()
	yield(rom_picker, "file_selected")
	sm64_handler.rom_filename = rom_picker.current_path

	sm64_handler.global_init()
	load_static_sufaces()
	load_moving_platform()

	for mario in get_tree().get_nodes_in_group("Mario"):
		mario.create()


func _physics_process(delta: float) -> void:
	time_since_last_tick += delta
	if time_since_last_tick < FPS_30_DELTA:
		return
	time_since_last_tick -= FPS_30_DELTA

	move_platform_in_lib()


func load_static_sufaces() -> void:
	var faces := PoolVector3Array()

	for mesh_instance in get_tree().get_nodes_in_group("static_surfaces"):
		var mesh_faces: PoolVector3Array = mesh_instance.get_mesh().get_faces()
		var offset: Vector3 = mesh_instance.global_transform.origin
		for i in range(mesh_faces.size()):
			mesh_faces[i] += offset
		faces.append_array(mesh_faces)

	sm64_handler.static_surfaces_load(faces)


func load_moving_platform() -> void:
	var moving_platform_mesh_faces: PoolVector3Array = moving_platform.get_mesh().get_faces()
	moving_platform_id = sm64_handler.surface_object_create(moving_platform_mesh_faces, moving_platform.global_transform.origin, moving_platform.rotation_degrees)
	moving_platform.get_node("AnimationPlayer").play("Move")


func move_platform_in_lib() -> void:
	if moving_platform_id >= 0:
		sm64_handler.surface_object_move(moving_platform_id, moving_platform.global_transform.origin, moving_platform.rotation_degrees)
