@icon("res://addons/libsm64-godot/mario/mario-godot.svg")
class_name SM64Mario
extends Node3D

## Node that instances a Mario into a scenario

## Value that represents Mario being at full health
const FULL_HEALTH = 0x0880
## Value that represents one health wedge
const HEALTH_WEDGE = 0x0100
## Special Caps mask
const SPECIAL_CAPS = SM64Mario.Caps.VANISH | SM64Mario.Caps.METAL | SM64Mario.Caps.WING

enum TickProcessMode {
	PHYSICS, ## Process tick during the physics process.
	IDLE,    ## Process tick during the idle process.
}

enum Caps {
	NORMAL = 0x1,
	VANISH = 0x2,
	METAL  = 0x4,
	WING   = 0x8,
}

signal action_changed(action: int)
signal flags_changed(flags: int)
signal health_changed(health: int)
signal health_wedges_changed(health_wedges: int)
# signal lives_changed(lives: int)

## Camera instance that follows Mario
@export var camera: Camera3D

## The process notification in which to tick.
@export var tick_process_mode := TickProcessMode.IDLE:
	set(value):
		tick_process_mode = value
		match tick_process_mode:
			TickProcessMode.PHYSICS:
				set_physics_process(true)
				set_process(false)
			TickProcessMode.IDLE:
				set_physics_process(false)
				set_process(true)

## if [code]true[/code], linearly interpolate Mario's mesh and transform
## from the SM64 engine's hardcoded 30 frames per second to the tick rate
## of the current [code]tick_process_mode[/code].
@export var interpolate := true:
	set(value):
		_internal.interpolate = value
		interpolate = value

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

var _internal := SM64MarioInternal.new()

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
		_internal.set_action(value)
		_action = value
## Mario's action as StringName
var action_name: StringName:
	get:
		return SM64MarioAction.to_action_name(_action)

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
		_internal.set_state(value)
		_flags = value

var _velocity := Vector3()
## Mario's velocity in the libsm64 world
var velocity: Vector3:
	get:
		return _velocity
	set(value):
		if _id < 0:
			return
		_internal.set_velocity(value)
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
		_internal.set_face_angle(value)
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
		_internal.set_health(value)
		_health = value

## Mario's amount of health wedges
var health_wedges: int:
	get:
		return _health >> 0x8 if _health > 0 else 0x0
	set(value):
		if _id < 0:
			return
		var new_health := value << 0x8 if value > 0 else 0x0
		_internal.set_health(new_health)
		_health = new_health

var _invicibility_time := 0.0
## Mario's invincibility time in seconds
var invicibility_time: float:
	get:
		return _invicibility_time
	set(value):
		if _id < 0:
			return
		_internal.set_invincibility(value)
		_invicibility_time = value

# var hurt_counter := 0

#var _lives := 4:
#	set(value):
#		_lives = value
#		lives_changed.emit(_lives)
## Mario's lives
#var lives: int:
#	get:
#		return _lives
#	set(value):
#		if _id < 0:
#			return
#		_internal.set_lives(value)
#		_lives = value

## Mario's water level
var water_level := -100000.0:
	set(value):
		if _id < 0:
			return
		_internal.set_water_level(value)
		water_level = value

## Mario's gas level
var gas_level := -100000.0:
	set(value):
		if _id < 0:
			return
		_internal.set_gas_level(value)
		gas_level = value

var _mesh_instance: MeshInstance3D
var _mesh: ArrayMesh
var _default_material := preload("res://addons/libsm64-godot/mario/mario_default_material.tres") as StandardMaterial3D
var _vanish_material := preload("res://addons/libsm64-godot/mario/mario_vanish_material.tres") as StandardMaterial3D
var _metal_material := preload("res://addons/libsm64-godot/mario/mario_metal_material.tres") as StandardMaterial3D
var _wing_material := preload("res://addons/libsm64-godot/mario/mario_wing_material.tres") as StandardMaterial3D
var _material: StandardMaterial3D
var _id := -1
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

	if tick_process_mode == TickProcessMode.PHYSICS:
		set_process(false)
	else:
		set_physics_process(false)


func _process(delta: float) -> void:
	_tick(delta)


func _physics_process(delta: float) -> void:
	_tick(delta)


## Create Mario (requires initializing the libsm64 via the global_init function)
func create() -> void:
	if SM64Global.is_init():
		if _id >= 0:
			delete()
		_id = _internal.mario_create(global_position, global_rotation)
		if _id < 0:
			return

		if not _default_material.detail_albedo:
			var detail_texture := SM64Global.get_mario_image_texture() as ImageTexture
			_default_material.detail_albedo = detail_texture
			_wing_material.detail_albedo = detail_texture
			_metal_material.detail_albedo = detail_texture
			_vanish_material.detail_albedo = detail_texture


## Delete mario inside the libsm64 world
func delete() -> void:
	if _id < 0:
		return
	_internal.mario_delete()
	_id = -1


## Teleport mario in the libsm64 world
func teleport(to_global_position: Vector3) -> void:
	if _id < 0:
		return
	_internal.set_position(to_global_position)
	global_position = to_global_position


## Set angle of mario in the libsm64 world
func set_angle(to_global_rotation: Vector3) -> void:
	if _id < 0:
		return
	_internal.set_angle(to_global_rotation)
	# global_rotation = to_global_rotation


## Set Mario's forward velocity in the libsm64 world
func set_forward_velocity(velocity: float) -> void:
	if _id < 0:
		return
	_internal.set_forward_velocity(velocity)


## Override the floor properties
# func set_floor_override(surface_properties: SM64SurfaceProperties) -> void:
# 	if _id < 0:
# 		return
# 	_internal.set_floor_override(surface_properties)


## Reset overriden floor properties
# func reset_floor_override() -> void:
# 	if _id < 0:
# 		return
# 	_internal.reset_floor_override()


## Make Mario take damage in amount of health wedges from a source position
func take_damage(damage: int, source_position: Vector3, big_knockback := false) -> void:
	if _id < 0:
		return
	_internal.take_damage(damage, source_position, big_knockback)


## Heal Mario a specific amount of wedges
func heal(wedges: int) -> void:
	if _id < 0:
		return
	_internal.heal(wedges)


## Kill Mario
func kill() -> void:
	if _id < 0:
		return
	_internal.kill()


## Equip special cap (see SM64Mario.Caps for values)
func interact_cap(cap: Caps, cap_time := 0.0, play_music := true) -> void:
	if _id < 0:
		return
	_internal.interact_cap(cap, cap_time, play_music)


## Extend current special cap time
func extend_cap(cap_time: float) -> void:
	if _id < 0:
		return
	_internal.extend_cap(cap_time)


func _tick(delta: float) -> void:
	if _id < 0:
		return

	_mario_input.stick = Input.get_vector(stick_left, stick_right, stick_up, stick_down)

	var look_direction := camera.global_transform.basis.z
	_mario_input.cam_look = Vector2(look_direction.x, look_direction.z)

	_mario_input.a = Input.is_action_pressed(input_a)
	_mario_input.b = Input.is_action_pressed(input_b)
	_mario_input.z = Input.is_action_pressed(input_z)

	var tick_output := _internal.tick(delta, _mario_input)

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

	_invicibility_time = tick_output.invinc_timer as float
	# hurt_counter = tick_output.hurt_counter as int

	# var new_lives := tick_output.num_lives as int
	# if new_lives != _lives:
	# 	_lives = new_lives

	match _flags & SPECIAL_CAPS:
		SM64Mario.Caps.VANISH:
			_material = _vanish_material
		SM64Mario.Caps.METAL:
			_material = _metal_material
		SM64Mario.Caps.WING:
			_material = _wing_material
		_:
			_material = _default_material

	var mesh_array := tick_output.mesh_array as Array
	_mesh.clear_surfaces()
	_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, mesh_array)
	_mesh_instance.set_surface_override_material(0, _material)
