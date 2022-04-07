extends Spatial


export var rom_filename := "baserom.us.z64"

var sm64_handler
var sm64_mario_id := -1

var time_since_last_tick := 0.0

onready var mario: KinematicBody = $Mario
onready var camera: Camera = $Camera

# Called when the node enters the scene tree for the first time.
func _ready():
	sm64_handler = load("res://SM64Handler.gdns").new()
	sm64_handler.global_init(rom_filename)
	load_static_sufaces()
	sm64_mario_id = sm64_handler.mario_create(mario.to_global(mario.translation))
	if sm64_mario_id < 0:
		print("Failed to create Mario")


func _physics_process(delta):

	var stick_x := Input.get_action_strength("mario_stick_right") - Input.get_action_strength("mario_stick_left")
	var stick_y := Input.get_action_strength("mario_stick_down") - Input.get_action_strength("mario_stick_up")
	var stick := Vector2(stick_x, stick_y)

	var camera_pos := camera.to_global(camera.translation)
	var cam_look := Vector2(camera_pos.x, camera_pos.z)

	var a := Input.is_action_pressed("mario_a")
	var b := Input.is_action_pressed("mario_b")
	var z := Input.is_action_pressed("mario_z")

	var inputs := {
		"cam_look": cam_look,
		"stick": stick,
		"a": a,
		"b": b,
		"z": z,
	}

	var tick_output: Dictionary = sm64_handler.mario_tick(sm64_mario_id, inputs)

	var mario_position: Vector3 = tick_output["position"]

	if not mario_position.is_equal_approx(Vector3.ZERO):
		mario.translate(mario_position - mario.to_global(mario.translation))


func load_static_sufaces() -> void:
	var platform: MeshInstance = $"../Platform"
	var plataform_mesh := platform.get_mesh()
	var faces := plataform_mesh.get_faces()
	for i in range(faces.size()):
		faces[i] = platform.global_transform.xform(faces[i])
	sm64_handler.static_surfaces_load(faces)
