class_name Mario3D
extends Node3D
@icon("res://addons/libsm64/mario/mario-godot.svg")

const FPS_30_DELTA = 1.0/30.0

## SM64Handler instance
@export var sm64_handler: Resource
## Camera instance that follows Mario
@export var camera: Camera3D

@export_group("Input Actions")
## Action equivalent to pushing the joystick to the left
@export var stick_left := "mario_stick_left"
## Action equivalent to pushing the joystick to the right
@export var stick_right := "mario_stick_right"
## Action equivalent to pushing the joystick upwards
@export var stick_up := "mario_stick_up"
## Action equivalent to pushing the joystick downwards
@export var stick_down := "mario_stick_down"
## Action equivalent to pushing the A button
@export var input_a := "mario_a"
## Action equivalent to pushing the B button
@export var input_b := "mario_b"
## Action equivalent to pushing the Z button
@export var input_z := "mario_z"

var mesh_instance: MeshInstance3D
var mesh: ArrayMesh
var material: StandardMaterial3D
var id := -1
var time_since_last_tick := 0.0
var _mario_input := SM64Input.new()

func _ready() -> void:
	mesh_instance = MeshInstance3D.new()
	add_child(mesh_instance)
	mesh_instance.set_as_top_level(true)
	mesh_instance.position = Vector3.ZERO

	mesh = ArrayMesh.new()
	mesh_instance.mesh = mesh


func _physics_process(delta: float) -> void:
	time_since_last_tick += delta
	if time_since_last_tick < FPS_30_DELTA:
		return
	time_since_last_tick -= FPS_30_DELTA

	if id < 0:
		return

	var stick_x := Input.get_action_strength(stick_left) - Input.get_action_strength(stick_right)
	var stick_y := Input.get_action_strength(stick_up) - Input.get_action_strength(stick_down)
	_mario_input.stick = Vector2(stick_x, stick_y)

	var look_direction := camera.get_global_transform().basis.z
	_mario_input.cam_look = Vector2(look_direction.x, look_direction.z)

	_mario_input.a = Input.is_action_pressed(input_a)
	_mario_input.b = Input.is_action_pressed(input_b)
	_mario_input.z = Input.is_action_pressed(input_z)

	var tick_output: Dictionary = sm64_handler.mario_tick(id, _mario_input)

	position = tick_output["position"] as Vector3

	var mesh_array: Array = tick_output["mesh_array"]
	mesh.clear_surfaces()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, mesh_array)
	mesh_instance.set_surface_override_material(0, material)


## Globally initialize the libsm64
func global_init() -> void:
	if sm64_handler and not sm64_handler.is_init():
		sm64_handler.global_init()


## Create Mario (requires initializing the libsm64 via the global_init function)
func create() -> void:
	if sm64_handler and sm64_handler.is_init():
		id = sm64_handler.mario_create(to_global(position))
		if id < 0:
			return

		var texture := ImageTexture.create_from_image(sm64_handler.get_mario_image())

		var texture_material := StandardMaterial3D.new()
		texture_material.albedo_texture = texture
		texture_material.transparency = true

		material = StandardMaterial3D.new()
		material.vertex_color_use_as_albedo = true
		material.next_pass = texture_material
