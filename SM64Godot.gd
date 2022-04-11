extends Spatial

const FPS_30_DELTA = 1.0/30.0

export var rom_filename := "baserom.us.z64"
export var sm64_handler: Resource

var sm64_mario_id := -1
var mario_material := SpatialMaterial.new()
var time_since_last_tick := 0.0

onready var mario: KinematicBody = $Mario
onready var mario_mesh: MeshInstance = $Mario/MeshArrayInstance
onready var camera: Camera = $Camera

# Called when the node enters the scene tree for the first time.
func _ready():
	mario_material.vertex_color_use_as_albedo = true
	var mario_image: Image = sm64_handler.global_init(rom_filename)
	var mario_texture := ImageTexture.new()
	mario_texture.create_from_image(mario_image)
	var mario_texture_material := SpatialMaterial.new()
	mario_texture_material.albedo_texture = mario_texture
	mario_texture_material.flags_transparent = true
	mario_material.next_pass = mario_texture_material
	load_static_sufaces()
	sm64_mario_id = sm64_handler.mario_create(mario.to_global(mario.translation))
	if sm64_mario_id < 0:
		print("Failed to create Mario")
	mario_mesh.set_as_toplevel(true)
	mario_mesh.translation = Vector3.ZERO


func _physics_process(delta):
	time_since_last_tick += delta
	if time_since_last_tick < FPS_30_DELTA:
		return
	time_since_last_tick -= FPS_30_DELTA

	var stick_x := Input.get_action_strength("mario_stick_left") - Input.get_action_strength("mario_stick_right")
	var stick_y := Input.get_action_strength("mario_stick_up") - Input.get_action_strength("mario_stick_down")
	var stick := Vector2(stick_x, stick_y)

	var look_direction := camera.get_global_transform().basis.z
	var cam_look := Vector2(look_direction.x, look_direction.z)

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
	mario.translation = mario_position

	var mesh_array: Array = tick_output["mesh_array"]

	mario_mesh.mesh.clear_surfaces()
	mario_mesh.mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, mesh_array)
	mario_mesh.set_surface_material(0, mario_material)


func load_static_sufaces() -> void:
	var platform: MeshInstance = $"../Platform"
	var wall: MeshInstance = $"../Wall"
	var platform_faces := platform.get_mesh().get_faces()
	var wall_faces = wall.get_mesh().get_faces()
	for i in range(wall_faces.size()):
		wall_faces[i] += wall.global_transform.origin
	for i in range(platform_faces.size()):
		platform_faces[i] += platform.global_transform.origin
	platform_faces.append_array(wall_faces)
	sm64_handler.static_surfaces_load(platform_faces)
