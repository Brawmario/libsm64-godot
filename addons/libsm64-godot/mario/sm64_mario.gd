@icon("res://addons/libsm64-godot/mario/mario-godot.svg")
class_name SM64Mario
extends Node3D


const FPS := 30.0
const DELTA := 1.0 / FPS

## Value that represents Mario being at full health
const FULL_HEALTH := 0x0880
## Value that represents one health wedge
const HEALTH_WEDGE := 0x100

enum Caps {
	NORMAL = SM64Handler.MARIO_CAPS_NORMAL,
	VANISH = SM64Handler.MARIO_CAPS_VANISH,
	METAL = SM64Handler.MARIO_CAPS_METAL,
	WING = SM64Handler.MARIO_CAPS_WING,
}

## Special Caps mask
const MARIO_SPECIAL_CAPS := Caps.VANISH | Caps.METAL | Caps.WING

signal action_changed(action: int)
signal flags_changed(flags: int)
signal health_changed(health: int)
signal health_wedges_changed(health_wedges: int)
signal lives_changed(lives: int)

## SM64Handler instance
@export var sm64_handler: SM64Handler
## Camera instance that follows Mario
@export var camera: Camera3D

@export_group("Input Actions")
## Action equivalent to pushing the joystick to the left
@export var stick_left := &"mario_stick_left"
## Action equivalent to pushing the joystick to the right
@export var stick_right := &"mario_stick_right"
## Action equivalent to pushing the joystick upwards
@export var stick_up := &"mario_stick_up"
## Action equivalent to pushing the joystick downwards
@export var stick_down := &"mario_stick_down"
## Action equivalent to pushing the A button
@export var input_a := &"mario_a"
## Action equivalent to pushing the B button
@export var input_b := &"mario_b"
## Action equivalent to pushing the Z button
@export var input_z := &"mario_z"

var _action := 0x0:
	set(value):
		_action = value
		action_changed.emit(_action)
## Mario's action flags
var action: int:
	get:
		return _action
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_action(_id, value)
		_action = value

var _flags := 0x0:
	set(value):
		_flags = value
		flags_changed.emit(_flags)
## Mario's state flags
var flags: int:
	get:
		return _flags
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_state(_id, value)
		_flags = value

var _velocity := Vector3()
## Mario's velocity in the libsm64 world
var velocity: Vector3:
	get:
		return _velocity
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_velocity(_id, value)
		_velocity = value

var _face_angle := 0.0:
	set(value):
		global_rotation.y = value
		_face_angle = value
## Mario's facing angle in radians
var face_angle: float:
	get:
		return _face_angle
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_angle(_id, value)
		_face_angle = value

var _health := FULL_HEALTH:
	set(value):
		_health = value
		health_changed.emit(_health)
		health_wedges_changed.emit(health_wedges)
## Mario's health (2 bytes, upper byte is the number of health wedges, lower byte portion of next wedge)
var health: int:
	get:
		return _health
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_health(_id, value)
		_health = value

## Mario's amount of health wedges
var health_wedges: int:
	get:
		return _health >> 0x8 if _health > 0 else 0x0
	set(value):
		if _id < 0:
			return
		var new_health := value << 0x8 if value > 0 else 0x0
		sm64_handler.set_mario_health(_id, new_health)
		_health = new_health

var _invicibility_time_frames := 0
## Mario's invincibility time in seconds
var invicibility_time: float:
	get:
		return _invicibility_time_frames / FPS
	set(value):
		if _id < 0:
			return
		_invicibility_time_frames = value * FPS
		sm64_handler.set_mario_invincibility(_id, _invicibility_time_frames)

var hurt_counter := 0

var _lives := 4:
	set(value):
		_lives = value
		lives_changed.emit(_lives)
## Mario's lives
var lives: int:
	get:
		return _lives
	set(value):
		if _id < 0:
			return
		sm64_handler.mario_set_lives(_id, value)
		_lives = value

## Mario's water level
var water_level := -100000.0:
	get:
		return water_level
	set(value):
		if _id < 0:
			return
		sm64_handler.set_mario_water_level(_id, value)
		water_level = value

var _mesh_instance: MeshInstance3D
var _mesh: ArrayMesh
var _default_material := preload("res://addons/libsm64-godot/mario/mario_default_material.tres") as StandardMaterial3D
var _vanish_material := preload("res://addons/libsm64-godot/mario/mario_vanish_material.tres") as StandardMaterial3D
var _metal_material := preload("res://addons/libsm64-godot/mario/mario_metal_material.tres") as StandardMaterial3D
var _wing_material := preload("res://addons/libsm64-godot/mario/mario_wing_material.tres") as StandardMaterial3D
var _material: StandardMaterial3D
var _id := -1
var _time_since_last_tick := 0.0
# FIXME: SM64Input stopped working in beta 15
var _mario_input := {}


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

	_mario_input.stick = Input.get_vector(stick_left, stick_right, stick_up, stick_down)

	var look_direction := camera.global_transform.basis.z
	_mario_input.cam_look = Vector2(look_direction.x, look_direction.z)

	_mario_input.a = Input.is_action_pressed(input_a)
	_mario_input.b = Input.is_action_pressed(input_b)
	_mario_input.z = Input.is_action_pressed(input_z)

	var tick_output := sm64_handler.mario_tick(_id, _mario_input)

	global_position = tick_output.position as Vector3
	_velocity = tick_output.velocity as Vector3
	_face_angle = tick_output.face_angle as float

	var new_health := tick_output.health as float
	if new_health != _health:
		_health = new_health

	var new_action := tick_output.action as int
	if new_action != _action:
		_action = new_action

	var new_flags := tick_output.flags as int
	if new_flags != _flags:
		_flags = new_flags

	_invicibility_time_frames = tick_output.invinc_timer as int
	hurt_counter = tick_output.hurt_counter as int

	var new_lives := tick_output.num_lives as int
	if new_lives != _lives:
		_lives = new_lives

	match _flags & MARIO_SPECIAL_CAPS:
		Caps.VANISH:
			_material = _vanish_material
		Caps.METAL:
			_material = _metal_material
		Caps.WING:
			_material = _wing_material
		_:
			_material = _default_material

	var mesh_array := tick_output.mesh_array as Array
	_mesh.clear_surfaces()
	_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, mesh_array)
	_mesh_instance.set_surface_override_material(0, _material)


## Create Mario (requires initializing the libsm64 via the global_init function)
func create() -> void:
	if sm64_handler and sm64_handler.is_init():
		if _id >= 0:
			delete()
		_id = sm64_handler.mario_create(global_position, global_rotation)
		if _id < 0:
			return

		if not _default_material.detail_albedo:
			var detail_texture := sm64_handler.get_mario_image_texture() as ImageTexture
			_default_material.detail_albedo = detail_texture
			_wing_material.detail_albedo = detail_texture
			_metal_material.detail_albedo = detail_texture
			_vanish_material.detail_albedo = detail_texture


## Delete mario inside the libsm64 world
func delete() -> void:
	if _id < 0:
		return
	sm64_handler.mario_delete(_id)
	_id = -1


## Get the current action name
func get_action_name() -> StringName:
	return SM64MarioAction.to_action_name(_action)


## Teleport mario in the libsm64 world
func teleport(to_global_position: Vector3) -> void:
	if _id < 0:
		return
	sm64_handler.set_mario_position(_id, to_global_position)
	global_position = to_global_position


## Set Mario's forward velocity in the libsm64 world
func set_forward_velocity(velocity: float) -> void:
	if _id < 0:
		return
	sm64_handler.set_mario_forward_velocity(_id, velocity)


## Override the floor properties
func set_floor_override(surface_properties: SM64SurfaceProperties) -> void:
	if _id < 0:
		return
	sm64_handler.set_mario_floor_override(_id, surface_properties)


## Reset overriden floor properties
func reset_floor_override() -> void:
	if _id < 0:
		return
	sm64_handler.reset_mario_floor_override(_id)


## Make Mario take damage in amount of health wedges from a source position
func take_damage(damage: int, source_position: Vector3, big_knockback := false) -> void:
	if _id < 0:
		return
	sm64_handler.mario_take_damage(_id, damage, source_position, big_knockback)


## Heal Mario a specific amount of wedges
func heal(wedges: int) -> void:
	if _id < 0:
		return
	sm64_handler.mario_heal(_id, wedges)


## Equip special cap
func interact_cap(cap: Caps, cap_time = 0.0, play_music := true) -> void:
	if _id < 0:
		return
	sm64_handler.mario_interact_cap(_id, cap, cap_time * FPS, play_music)


## Extend current special cap time
func extend_cap(cap_time: float) -> void:
	if _id < 0:
		return
	sm64_handler.mario_extend_cap(_id, cap_time * FPS)
