class_name SM64Mario
extends Node3D
@icon("res://addons/libsm64/mario/mario-godot.svg")


const FPS := 30.0
const DELTA := 1.0 / FPS

## SM64Handler instance
@export var sm64_handler: SM64Handler
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

var _velocity := Vector3()
## Set Mario's velocity in the libsm64 world
var velocity: Vector3:
	get:
		return _velocity
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_velocity(_id, value)
		_velocity = value

var _health := 0x0880
## Set Mario's health
var health: int:
	get:
		return _health
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_health(_id, value)
		_health = value

var _invicibility_time := 0.0
## Set Mario's invincibility time in seconds
var invicibility_time: float:
	get:
		return _invicibility_time
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_invincibility(_id, value * FPS)
		_invicibility_time = value

var hurt_counter := 0
var lives := 4

var _water_level := -100000.0
## Set Mario's water level
var water_level: float:
	get:
		return _water_level
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_water_level(_id, value)
		_water_level = value


var _mesh_instance: MeshInstance3D
var _mesh: ArrayMesh
var _material: StandardMaterial3D
var _id := -1
var _time_since_last_tick := 0.0
var _mario_input := SM64Input.new()
var _mario_image: Image


func _ready() -> void:
	_mesh_instance = MeshInstance3D.new()
	add_child(_mesh_instance)
	_mesh_instance.top_level = true
	_mesh_instance.position = Vector3.ZERO
	_mesh_instance.rotation = Vector3.ZERO

	_mesh = ArrayMesh.new()
	_mesh_instance.mesh = _mesh


func _physics_process(delta: float) -> void:
	_time_since_last_tick += delta
	if _time_since_last_tick < DELTA:
		return
	_time_since_last_tick -= DELTA

	if _id < 0:
		return

	var stick_x := Input.get_action_strength(stick_left) - Input.get_action_strength(stick_right)
	var stick_y := Input.get_action_strength(stick_up) - Input.get_action_strength(stick_down)
	_mario_input.stick = Vector2(stick_x, stick_y)

	var look_direction := camera.global_transform.basis.z
	_mario_input.cam_look = Vector2(look_direction.x, look_direction.z)

	_mario_input.a = Input.is_action_pressed(input_a)
	_mario_input.b = Input.is_action_pressed(input_b)
	_mario_input.z = Input.is_action_pressed(input_z)

	var tick_output := sm64_handler.mario_tick(_id, _mario_input)

	global_position = tick_output["position"] as Vector3
	_velocity = tick_output["velocity"] as Vector3
	global_rotation.y = tick_output["face_angle"] as float
	_health = tick_output["health"] as float
	_invicibility_time = (tick_output["invinc_timer"] as int) / FPS
	hurt_counter = tick_output["hurt_counter"] as int
	lives = tick_output["num_lives"] as int

	var mesh_array := tick_output["mesh_array"] as Array
	_mesh.clear_surfaces()
	_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, mesh_array)
	_mesh_instance.set_surface_override_material(0, _material)


## Globally initialize the libsm64
func global_init() -> void:
	if sm64_handler and not sm64_handler.is_init():
		sm64_handler.global_init()


## Create Mario (requires initializing the libsm64 via the global_init function)
func create() -> void:
	if sm64_handler and sm64_handler.is_init():
		_id = sm64_handler.mario_create(global_position, global_rotation)
		if _id < 0:
			return

		_mario_image = sm64_handler.get_mario_image()
		var texture := ImageTexture.create_from_image(_mario_image)

		var texture_material := StandardMaterial3D.new()
		texture_material.albedo_texture = texture
		texture_material.transparency = true

		_material = StandardMaterial3D.new()
		_material.vertex_color_use_as_albedo = true
		_material.next_pass = texture_material


## Delete mario inside the libsm64 world
func delete() -> void:
	if _id < 0:
		return
	sm64_handler.mario_delete(_id)


## Teleport mario in the libsm64 world
func teleport(to_global_position: Vector3) -> void:
	if _id < 0:
		return
	sm64_handler.set_mario_position(_id, to_global_position)
	global_position = to_global_position


## Set Mario's facing angle
func set_face_angle(angle: float) -> void:
	if _id < 0:
		return
	sm64_handler.set_mario_angle(_id, angle)
	global_rotation.y = angle


## Set Mario's forward velocity in the libsm64 world
func set_forward_velocity(vel: float) -> void:
	if _id < 0:
		return
	sm64_handler.set_mario_forward_velocity(_id, vel)


## Make Mario take damage
func take_damage(damage: int, source_position: Vector3, subtype := 0) -> void:
	if _id < 0:
		return
	sm64_handler.mario_take_damage(_id, damage, source_position, subtype)


## Heal Mario
func heal(value: int) -> void:
	if _id < 0:
		return
	sm64_handler.mario_heal(_id, value)
