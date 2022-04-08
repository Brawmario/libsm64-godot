extends Spatial


export var rom_filename := "baserom.us.z64"

var sm64_handler
var sm64_mario_id := -1
var mario_material := SpatialMaterial.new()
var time_since_last_tick := 0.0

onready var mario: KinematicBody = $Mario
onready var mario_mesh: MeshInstance = $Mario/MeshArrayInstance
onready var camera: Camera = $Camera

# Called when the node enters the scene tree for the first time.
func _ready():
	sm64_handler = load("res://SM64Handler.gdns").new()
	var mario_image: Image = sm64_handler.global_init(rom_filename)
	var mario_texture := ImageTexture.new()
	mario_texture.create_from_image(mario_image)
	mario_material.albedo_texture = mario_texture
	load_static_sufaces()
	sm64_mario_id = sm64_handler.mario_create(mario.to_global(mario.translation))
	if sm64_mario_id < 0:
		print("Failed to create Mario")


func _physics_process(delta):
	# Force 30 fps physics
	time_since_last_tick += delta
	if time_since_last_tick < 1.0/30.0:
		return
	time_since_last_tick -= 1.0/30.0

	var stick_x := Input.get_action_strength("mario_stick_left") - Input.get_action_strength("mario_stick_right")
	var stick_y := Input.get_action_strength("mario_stick_up") - Input.get_action_strength("mario_stick_down")
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

	# FIXME: Makes Mario vibrate too much
	#var mario_position: Vector3 = tick_output["position"]
	#if not mario_position.is_equal_approx(Vector3.ZERO):
	#	mario.translate(mario_position - mario.to_global(mario.translation))

	var mesh_array: Array = tick_output["mesh_array"]

	# FIXME: Colors wrong?
	mario_mesh.mesh.clear_surfaces()
	mario_mesh.mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, mesh_array)
	# FIXME: Add these texures after figuiring out the colors
	# mario_mesh.set_surface_material(0, mario_material)


func load_static_sufaces() -> void:
	var platform: MeshInstance = $"../Platform"
	var plataform_mesh := platform.get_mesh()
	var faces := plataform_mesh.get_faces()
	for i in range(faces.size()):
		faces[i] = platform.global_transform.xform(faces[i])
	# Flip vertex order
	for i in range(faces.size() / 3):
		var temp := faces[3*i+0]
		faces[3*i+0] = faces[3*i+1]
		faces[3*i+1] = temp
	sm64_handler.static_surfaces_load(faces)
