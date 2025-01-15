@icon("res://addons/libsm64-godot/libsm64_mario/libsm64_mario.svg")
class_name LibSM64Mario
extends Node3D

## Node that instances a Mario into a scenario

## Value that represents Mario being at full health
const FULL_HEALTH = 0x0880
## Value that represents one health wedge
const HEALTH_WEDGE = 0x0100


signal action_changed(action: LibSM64.ActionFlags)
signal flags_changed(flags: LibSM64.MarioFlags)
signal health_changed(health: int)
signal health_wedges_changed(health_wedges: int)

## Camera instance that follows Mario
@export var camera: Camera3D

## if [code]true[/code], linearly interpolate Mario and transform
## from the SM64 engine's hardcoded 30 frames per second to the tick rate
## of the current [code]tick_process_mode[/code].
@export var interpolate := true

@export_group("Mario Inputs Actions", "mario_inputs_")
## Action equivalent to pushing the joystick to the left
@export var mario_inputs_stick_left := &"libsm64_mario_inputs_stick_left"
## Action equivalent to pushing the joystick to the right
@export var mario_inputs_stick_right := &"libsm64_mario_inputs_stick_right"
## Action equivalent to pushing the joystick upwards
@export var mario_inputs_stick_up := &"libsm64_mario_inputs_stick_up"
## Action equivalent to pushing the joystick downwards
@export var mario_inputs_stick_down := &"libsm64_mario_inputs_stick_down"
## Action equivalent to pushing the A button
@export var mario_inputs_button_a := &"libsm64_mario_inputs_button_a"
## Action equivalent to pushing the B button
@export var mario_inputs_button_b := &"libsm64_mario_inputs_button_b"
## Action equivalent to pushing the Z button
@export var mario_inputs_button_z := &"libsm64_mario_inputs_button_z"

var _id := -1
## Mario's internal [code]libsm64[/code] ID
var id: int:
	get:
		return _id

var _action := LibSM64.ActionFlags.ACT_UNINITIALIZED:
	set(value):
		_action = value
		action_changed.emit(_action)
## Mario's action flags
var action: LibSM64.ActionFlags:
	get:
		return _action
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_action(_id, value)
		_action = value

## Mario's action as StringName
var action_name: StringName:
	get:
		return LibSM64MarioAction.to_action_name(_action)

var _flags := LibSM64.MarioFlags.MARIO_NORMAL_CAP:
	set(value):
		_flags = value
		flags_changed.emit(_flags)
## Mario's state flags
var flags: LibSM64.MarioFlags:
	get:
		return _flags
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_state(_id, value)
		_flags = value

var _velocity := Vector3()
## Mario's velocity in the libsm64 world
var velocity: Vector3:
	get:
		return _velocity
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_velocity(_id, value)
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
		LibSM64.set_mario_face_angle(_id, value)
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
		LibSM64.set_mario_health(_id, value)
		_health = value

## Mario's amount of health wedges
var health_wedges: int:
	get:
		return _health >> 0x8 if _health > 0 else 0x0
	set(value):
		if _id < 0:
			return
		var new_health := value << 0x8 if value > 0 else 0x0
		LibSM64.set_mario_health(_id, new_health)
		_health = new_health

var _invicibility_time := 0.0
## Mario's invincibility time in seconds
var invicibility_time: float:
	get:
		return _invicibility_time
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_invincibility(_id, value)
		_invicibility_time = value

## Mario's water level
var water_level := -100000.0:
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_water_level(_id, value)
		water_level = value

## Mario's gas level
var gas_level := -100000.0:
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_gas_level(_id, value)
		gas_level = value

var _mesh_instance: MeshInstance3D
var _mesh: ArrayMesh

var _mario_internal := LibSM64MarioInternal.new()

var _default_material := preload("res://addons/libsm64-godot/libsm64_mario/libsm64_mario_default_material.tres") as StandardMaterial3D
var _vanish_material := preload("res://addons/libsm64-godot/libsm64_mario/libsm64_mario_vanish_material.tres") as StandardMaterial3D
var _metal_material := preload("res://addons/libsm64-godot/libsm64_mario/libsm64_mario_metal_material.tres") as StandardMaterial3D
var _wing_material := preload("res://addons/libsm64-godot/libsm64_mario/libsm64_mario_wing_material.tres") as StandardMaterial3D

var _time_since_last_tick := 0.0


func _ready() -> void:
	_mesh_instance = MeshInstance3D.new()
	add_child(_mesh_instance)
	_mesh_instance.top_level = true
	_mesh_instance.position = Vector3.ZERO
	_mesh_instance.rotation = Vector3.ZERO

	_mesh = ArrayMesh.new()
	_mesh_instance.mesh = _mesh


func _process(delta: float) -> void:
	_time_since_last_tick += delta
	while _time_since_last_tick >= LibSM64.sm64_timestep_interval:
		_tick()
		_time_since_last_tick -= LibSM64.sm64_timestep_interval

	var lerp_t = _time_since_last_tick / LibSM64.sm64_timestep_interval

	var mario_state: LibSM64MarioState
	if interpolate:
		mario_state = _mario_internal.interpolate_mario_state(lerp_t)
	else:
		mario_state = _mario_internal.mario_state_current

	global_position = mario_state.position
	_velocity = mario_state.velocity
	_face_angle = mario_state.face_angle

	var new_health := mario_state.health
	if new_health != _health:
		_health = new_health

	var new_action := mario_state.action
	if new_action != _action:
		_action = new_action

	var new_flags := mario_state.flags
	if new_flags != _flags:
		_flags = new_flags

	_invicibility_time = mario_state.invincibility_time

	var material: StandardMaterial3D
	match _flags & LibSM64.MARIO_SPECIAL_CAPS:
		LibSM64.MARIO_VANISH_CAP :
			material = _vanish_material
		LibSM64.MARIO_METAL_CAP :
			material = _metal_material
		LibSM64.MARIO_WING_CAP:
			material = _wing_material
		_:
			material = _default_material

	var array_mesh_triangles: Array
	if interpolate:
		array_mesh_triangles = _mario_internal.interolate_array_mesh_triangles(lerp_t)
	else:
		array_mesh_triangles = _mario_internal.array_mesh_triangles_current

	if not array_mesh_triangles[ArrayMesh.ARRAY_VERTEX].is_empty():
		_mesh.clear_surfaces()
		_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, array_mesh_triangles)
		_mesh_instance.set_surface_override_material(0, material)


## Create Mario (requires initializing the libsm64 via the global_init function)
func create() -> void:
	if _id >= 0:
		delete()

	_id = LibSM64.mario_create(global_position)
	if _id < 0:
		return

	_mario_internal.mario_state_current.position = global_position
	_mario_internal.mario_state_previous.position = global_position

	if not _default_material.albedo_texture:
		_default_material.albedo_texture = LibSM64Global.mario_texture
		_wing_material.albedo_texture = LibSM64Global.mario_texture
		_metal_material.albedo_texture = LibSM64Global.mario_texture
		_vanish_material.albedo_texture = LibSM64Global.mario_texture


## Delete mario inside the libsm64 world
func delete() -> void:
	if _id < 0:
		return
	LibSM64.mario_delete(_id)
	_id = -1


## Teleport mario in the libsm64 world
func teleport(to_global_position: Vector3) -> void:
	if _id < 0:
		return
	LibSM64.set_mario_position(_id, to_global_position)
	global_position = to_global_position
	_mario_internal.mario_state_current.position = to_global_position

	# Reset interpolation
	_mario_internal.mario_state_previous = _mario_internal.mario_state_current
	_mario_internal.array_mesh_triangles_previous = _mario_internal.array_mesh_triangles_current


## Set angle of mario in the libsm64 world
func set_angle(to_global_rotation: Quaternion) -> void:
	if _id < 0:
		return
	LibSM64.set_mario_angle(_id, to_global_rotation)
	# TODO: check this out
	# global_rotation = to_global_rotation


## Set Mario's forward velocity in the libsm64 world
func set_forward_velocity(velocity: float) -> void:
	if _id < 0:
		return
	LibSM64.set_mario_forward_velocity(_id, velocity)


## Make Mario take damage in amount of health wedges from a source position
func take_damage(damage: int, subtype: int, source_position: Vector3) -> void:
	if _id < 0:
		return
	LibSM64.mario_take_damage(_id, damage, subtype, source_position)


## Heal Mario a specific amount of wedges
func heal(wedges: int) -> void:
	if _id < 0:
		return
	LibSM64.mario_heal(_id, wedges)


## Kill Mario
func kill() -> void:
	if _id < 0:
		return
	LibSM64.mario_kill(_id)


## Equip special cap (see LibSM64.MarioFlags for values)
func interact_cap(cap_flag: LibSM64.MarioFlags, cap_time := 0.0, play_music := true) -> void:
	if _id < 0:
		return
	LibSM64.mario_interact_cap(_id, cap_flag, cap_time, play_music)


## Extend current special cap time
func extend_cap(cap_time: float) -> void:
	if _id < 0:
		return
	LibSM64.mario_extend_cap(_id, cap_time)


func _make_mario_inputs() -> LibSM64MarioInputs:
	var mario_inputs := LibSM64MarioInputs.new()

	mario_inputs.stick = Input.get_vector(mario_inputs_stick_left, mario_inputs_stick_right, mario_inputs_stick_up, mario_inputs_stick_down)

	var look_direction := camera.global_transform.basis.z
	mario_inputs.cam_look = Vector2(look_direction.x, look_direction.z)

	mario_inputs.button_a = Input.is_action_pressed(mario_inputs_button_a)
	mario_inputs.button_b = Input.is_action_pressed(mario_inputs_button_b)
	mario_inputs.button_z = Input.is_action_pressed(mario_inputs_button_z)

	return mario_inputs


func _tick() -> void:
	if _id < 0:
		return

	var mario_inputs := _make_mario_inputs()

	_mario_internal.mario_state_previous = _mario_internal.mario_state_current
	_mario_internal.array_mesh_triangles_previous = _mario_internal.array_mesh_triangles_current

	var mario_tick_output := LibSM64.mario_tick(_id, mario_inputs)

	_mario_internal.mario_state_current = mario_tick_output[0]
	_mario_internal.array_mesh_triangles_current = mario_tick_output[1]
