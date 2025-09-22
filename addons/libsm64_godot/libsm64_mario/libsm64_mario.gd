@icon("res://addons/libsm64_godot/libsm64_mario/libsm64_mario.svg")
class_name LibSM64Mario
extends Node3D
## Node that instances a Mario into a scenario and provides an interface to control it.
##
## This class exposes Mario's state, actions, and interactions within the [code]libsm64[/code] world.
## It includes methods for creating, deleting, teleporting, and controlling Mario,
## as well as handling his health, velocity, and special abilities.


## Value that represents Mario being at full health.
const FULL_HEALTH = 0x0880
## Value that represents one health wedge.
const HEALTH_WEDGE = 0x0100


## Emitted when Mario's action changes.
signal action_changed(action: LibSM64.ActionFlags)
## Emitted when Mario's flags change.
signal flags_changed(flags: LibSM64.MarioFlags)
## Emitted when Mario's particle flags change.
signal particle_flags_changed(particle_flags: LibSM64.ParticleFlags)
## Emitted when Mario's health changes.
signal health_changed(health: int)
## Emitted when Mario's health wedges change.
signal health_wedges_changed(health_wedges: int)

## Camera instance that follows Mario. Necessary for resolving Mario's inputs.
@export var camera: Camera3D

## if [code]true[/code], linearly interpolate Mario and transform
## from the SM64 engine's hardcoded 30 frames per second to the tick rate
## of the current [code]tick_process_mode[/code].
@export var interpolate := true

@export_group("Mario Inputs Actions", "mario_inputs_")
## Action equivalent to pushing the joystick to the left.
@export var mario_inputs_stick_left := &"libsm64_mario_inputs_stick_left"
## Action equivalent to pushing the joystick to the right.
@export var mario_inputs_stick_right := &"libsm64_mario_inputs_stick_right"
## Action equivalent to pushing the joystick upwards.
@export var mario_inputs_stick_up := &"libsm64_mario_inputs_stick_up"
## Action equivalent to pushing the joystick downwards.
@export var mario_inputs_stick_down := &"libsm64_mario_inputs_stick_down"
## Action equivalent to pushing the A button.
@export var mario_inputs_button_a := &"libsm64_mario_inputs_button_a"
## Action equivalent to pushing the B button.
@export var mario_inputs_button_b := &"libsm64_mario_inputs_button_b"
## Action equivalent to pushing the Z button.
@export var mario_inputs_button_z := &"libsm64_mario_inputs_button_z"

var _id := -1
## Mario's internal [code]libsm64[/code] ID. If [code]-1[/code], Mario hasn't been created.
var id: int:
	get:
		return _id

var _action := LibSM64.ActionFlags.ACT_UNINITIALIZED:
	set(value):
		if value != _action:
			_action = value
			action_changed.emit(_action)
## The current action state of Mario. This determines his animation and behavior state (e.g., jumping, running, swimming).
var action: LibSM64.ActionFlags:
	get:
		return _action
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_action(_id, value)
		_action = value

## The current action state of Mario as a [StringName].
var action_name: StringName:
	get:
		return _to_action_name(_action)

var _anim_id := LibSM64.MarioAnimID.MARIO_ANIM_SLOW_LEDGE_GRAB
## The ID of the current animation being played for Mario. This corresponds to specific animations defined in [enum LibSM64.MarioAnimID].
var anim_id: LibSM64.MarioAnimID:
	get:
		return _anim_id
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_animation(_id, value)
		_anim_id = value

var _anim_frame := 0
## The current frame of Mario's animation. This value is used to track the progress of the current animation sequence.
var anim_frame: int:
	get:
		return _anim_frame
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_anim_frame(_id, value)
		_anim_frame = value

var _flags := 0 as LibSM64.MarioFlags:
	set(value):
		if value != _flags:
			_flags = value
			flags_changed.emit(_flags)
## Various status flags that affect Mario's state and behavior (e.g., cap status, metal state, invincibility).
var flags: LibSM64.MarioFlags:
	get:
		return _flags
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_state(_id, value)
		_flags = value

var _particle_flags := 0 as LibSM64.ParticleFlags:
	set(value):
		if value != _particle_flags:
			_particle_flags = value
			particle_flags_changed.emit(_particle_flags)
## Flags indicating which particles should be spawned around Mario (e.g., dust, bubbles, sparkles).
var particle_flags: LibSM64.ParticleFlags:
	get:
		return _particle_flags

var _velocity := Vector3()
## Mario's current velocity vector, indicating both the direction and speed of his movement.
var velocity: Vector3:
	get:
		return _velocity
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_velocity(_id, value)
		_velocity = value
		if _mario_interpolator.mario_state_current:
			_mario_interpolator.mario_state_current.velocity = _velocity
			_mario_interpolator.mario_state_previous.velocity = _velocity

var _forward_velocity := 0.0
## Mario's current forward velocity (in meters per second). This value indicates how fast Mario is moving in the direction he is facing.
var forward_velocity: float:
	get:
		return _forward_velocity
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_forward_velocity(_id, value)
		_forward_velocity = value
		if _mario_interpolator.mario_state_current:
			_mario_interpolator.mario_state_current.forward_velocity = _forward_velocity
			_mario_interpolator.mario_state_previous.forward_velocity = _forward_velocity

var _face_angle := 0.0:
	set(value):
		global_rotation.y = value
		_face_angle = value
## The angle (in radians) that Mario is facing in the horizontal plane. [code]0.0[/code] points along the negative X axis.
var face_angle: float:
	get:
		return _face_angle
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_face_angle(_id, value)
		_face_angle = value
		if _mario_interpolator.mario_state_current:
			_mario_interpolator.mario_state_current.face_angle = _face_angle
			_mario_interpolator.mario_state_previous.face_angle = _face_angle

var _health := FULL_HEALTH:
	set(value):
		if value != _health:
			_health = value
			health_changed.emit(_health)
			health_wedges_changed.emit(health_wedges)
## Mario's current health value. This is a 16 bit value: upper byte is the number of health wedges; the lower byte is the portion of next wedge.
var health: int:
	get:
		return _health
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_health(_id, value)
		_health = value

## Mario's current health value in health wedges. This is a number between [code]0[/code] and [code]8[/code].
var health_wedges: int:
	get:
		return _health >> 0x8 if _health > 0 else 0x0
	set(value):
		if _id < 0:
			return
		var new_health := value << 0x8 if value > 0 else 0x0
		LibSM64.set_mario_health(_id, new_health)
		_health = new_health

var _invincibility_time := 0.0
## The remaining time (in seconds) of Mario's invincibility state.
var invincibility_time: float:
	get:
		return _invincibility_time
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_invincibility(_id, value)
		_invincibility_time = value
		if _mario_interpolator.mario_state_current:
			_mario_interpolator.mario_state_current.invincibility_time = _invincibility_time
			_mario_interpolator.mario_state_previous.invincibility_time = _invincibility_time

var _water_level_in_libsm64 := -100000
## The height of the water level in the [code]libsm64[/code] world for this Mario instance. If Mario is below this level, he will be considered swimming.
var water_level: float:
	get:
		return float(_water_level_in_libsm64) / LibSM64.scale_factor
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_water_level(_id, value)
		_water_level_in_libsm64 = int(value * LibSM64.scale_factor)

var _gas_level_in_libsm64 := -100000
## The height of the gas level in the [code]libsm64[/code] world for this Mario instance. If Mario is below this level, he will start coughing and take damage, eventually choking to death.
var gas_level: float:
	get:
		return float(_gas_level_in_libsm64) / LibSM64.scale_factor
	set(value):
		if _id < 0:
			return
		LibSM64.set_mario_gas_level(_id, value)
		_gas_level_in_libsm64 = int(value * LibSM64.scale_factor)

var _mesh_instance: MeshInstance3D
var _mesh: ArrayMesh

var _mario_interpolator := LibSM64MarioInterpolator.new()

var _default_material := preload("res://addons/libsm64_godot/libsm64_mario/libsm64_mario_default_material.tres") as StandardMaterial3D
var _vanish_material := preload("res://addons/libsm64_godot/libsm64_mario/libsm64_mario_vanish_material.tres") as StandardMaterial3D
var _metal_material := preload("res://addons/libsm64_godot/libsm64_mario/libsm64_mario_metal_material.tres") as StandardMaterial3D
var _wing_material := preload("res://addons/libsm64_godot/libsm64_mario/libsm64_mario_wing_material.tres") as StandardMaterial3D

var _time_since_last_tick := 0.0
var _last_tick_usec := Time.get_ticks_usec()
var _reset_interpolation_next_tick := false


func _ready() -> void:
	## Mario's mesh vertices are in global space.
	_mesh_instance = MeshInstance3D.new()
	add_child(_mesh_instance)
	_mesh_instance.top_level = true
	_mesh_instance.position = Vector3.ZERO
	_mesh_instance.rotation = Vector3.ZERO

	_mesh = ArrayMesh.new()
	_mesh_instance.mesh = _mesh


func _process(delta: float) -> void:
	if _id < 0:
		return

	var lerp_t = (Time.get_ticks_usec() - _last_tick_usec) / (LibSM64.tick_delta_time * 1000000.0)

	_update_lerped_members_from_mario_state(lerp_t)
	_update_mesh(lerp_t)


func _update_lerped_members_from_mario_state(lerp_t: float) -> void:
	var mario_state: LibSM64MarioState
	if interpolate:
		mario_state = _mario_interpolator.interpolate_mario_state(lerp_t)
	else:
		mario_state = _mario_interpolator.mario_state_current

	global_position = mario_state.position
	_velocity = mario_state.velocity
	_forward_velocity = mario_state.forward_velocity
	_face_angle = mario_state.face_angle
	_invincibility_time = mario_state.invincibility_time


func _update_mesh(lerp_t: float) -> void:
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
		array_mesh_triangles = _mario_interpolator.interpolate_array_mesh_triangles(lerp_t)
	else:
		array_mesh_triangles = _mario_interpolator.array_mesh_triangles_current

	if not array_mesh_triangles.is_empty() and not array_mesh_triangles[ArrayMesh.ARRAY_VERTEX].is_empty():
		_mesh.clear_surfaces()
		_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, array_mesh_triangles)
		_mesh_instance.set_surface_override_material(0, material)


func _physics_process(delta):
	if _id < 0:
		return

	_time_since_last_tick += delta
	while _time_since_last_tick >= LibSM64.tick_delta_time:
		_tick()
		_last_tick_usec = Time.get_ticks_usec()
		_time_since_last_tick -= LibSM64.tick_delta_time
		_update_non_lerped_members_from_mario_state()


func _update_non_lerped_members_from_mario_state() -> void:
	_health = _mario_interpolator.mario_state_current.health
	_action = _mario_interpolator.mario_state_current.action
	_anim_id = _mario_interpolator.mario_state_current.anim_id
	_anim_frame = _mario_interpolator.mario_state_current.anim_frame
	_flags = _mario_interpolator.mario_state_current.flags
	_particle_flags = _mario_interpolator.mario_state_current.particle_flags


## Create Mario (requires initializing [code]libsm64[/code] via the [method LibSM64Global.init] method).
func create() -> void:
	if _id >= 0:
		delete()

	_id = LibSM64.mario_create(global_position)
	if _id < 0:
		return
	face_angle = global_rotation.y

	_mario_interpolator.mario_state_current = LibSM64MarioState.new()
	_mario_interpolator.mario_state_current.position = global_position
	_mario_interpolator.mario_state_current.face_angle = _face_angle
	_mario_interpolator.mario_state_current.health = FULL_HEALTH
	_mario_interpolator.array_mesh_triangles_current = [PackedVector3Array(), PackedVector3Array(), null, PackedColorArray(), PackedVector2Array(), null, null, null, null, null, null, null, null]

	_mario_interpolator.mario_state_previous = _mario_interpolator.mario_state_current
	_mario_interpolator.array_mesh_triangles_previous = _mario_interpolator.array_mesh_triangles_current

	reset_interpolation()

	_default_material.next_pass.albedo_texture = LibSM64Global.mario_texture
	_wing_material.next_pass.albedo_texture = LibSM64Global.mario_texture
	_metal_material.next_pass.albedo_texture = LibSM64Global.mario_texture
	_vanish_material.next_pass.albedo_texture = LibSM64Global.mario_texture


## Delete mario inside the [code]libsm64[/code] world.
func delete() -> void:
	if _id < 0:
		return
	LibSM64.mario_delete(_id)
	_id = -1


## Teleport mario in the [code]libsm64[/code] world.
func teleport(to_global_position: Vector3) -> void:
	if _id < 0:
		return
	LibSM64.set_mario_position(_id, to_global_position)
	global_position = to_global_position
	_mario_interpolator.mario_state_current.position = global_position
	_mario_interpolator.mario_state_previous.position = global_position
	reset_interpolation()


## Set angle of mario in the [code]libsm64[/code] world.
func set_angle(to_global_rotation: Quaternion) -> void:
	if _id < 0:
		return
	LibSM64.set_mario_angle(_id, to_global_rotation)
	_face_angle = to_global_rotation.get_euler().y
	_mario_interpolator.mario_state_current.face_angle = _face_angle
	_mario_interpolator.mario_state_previous.face_angle = _face_angle
	reset_interpolation()


## Make Mario take damage in amount of health wedges from a source position.
func take_damage(damage: int, subtype: int, source_position: Vector3) -> void:
	if _id < 0:
		return
	LibSM64.mario_take_damage(_id, damage, subtype, source_position)


## Heal Mario a specific amount of wedges.
func heal(wedges: int) -> void:
	if _id < 0:
		return
	LibSM64.mario_heal(_id, wedges)


## Kill Mario.
func kill() -> void:
	if _id < 0:
		return
	LibSM64.mario_kill(_id)


## Equip special cap.
func interact_cap(cap_flag: LibSM64.MarioFlags, cap_time := 0.0, play_music := true) -> void:
	if _id < 0:
		return
	LibSM64.mario_interact_cap(_id, cap_flag, cap_time, play_music)


## Extend current special cap time.
func extend_cap(cap_time: float) -> void:
	if _id < 0:
		return
	LibSM64.mario_extend_cap(_id, cap_time)


## Reset interpolation next tick.
func reset_interpolation() -> void:
	_reset_interpolation_next_tick = true


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

	_mario_interpolator.mario_state_previous = _mario_interpolator.mario_state_current
	_mario_interpolator.array_mesh_triangles_previous = _mario_interpolator.array_mesh_triangles_current

	var mario_tick_output := LibSM64.mario_tick(_id, mario_inputs)

	_mario_interpolator.mario_state_current = mario_tick_output[0]
	_mario_interpolator.array_mesh_triangles_current = mario_tick_output[1]

	if _reset_interpolation_next_tick:
		_mario_interpolator.mario_state_previous = _mario_interpolator.mario_state_current
		_mario_interpolator.array_mesh_triangles_previous = _mario_interpolator.array_mesh_triangles_current
		_reset_interpolation_next_tick = false


static func _to_action_name(action: int) -> StringName:
	match action:
		LibSM64.ACT_IDLE:
			return &"ACT_IDLE"
		LibSM64.ACT_START_SLEEPING:
			return &"ACT_START_SLEEPING"
		LibSM64.ACT_SLEEPING:
			return &"ACT_SLEEPING"
		LibSM64.ACT_WAKING_UP:
			return &"ACT_WAKING_UP"
		LibSM64.ACT_PANTING:
			return &"ACT_PANTING"
		LibSM64.ACT_HOLD_PANTING_UNUSED:
			return &"ACT_HOLD_PANTING_UNUSED"
		LibSM64.ACT_HOLD_IDLE:
			return &"ACT_HOLD_IDLE"
		LibSM64.ACT_HOLD_HEAVY_IDLE:
			return &"ACT_HOLD_HEAVY_IDLE"
		LibSM64.ACT_STANDING_AGAINST_WALL:
			return &"ACT_STANDING_AGAINST_WALL"
		LibSM64.ACT_COUGHING:
			return &"ACT_COUGHING"
		LibSM64.ACT_SHIVERING:
			return &"ACT_SHIVERING"
		LibSM64.ACT_IN_QUICKSAND:
			return &"ACT_IN_QUICKSAND"
		LibSM64.ACT_UNKNOWN_0002020E:
			return &"ACT_UNKNOWN_0002020E"
		LibSM64.ACT_CROUCHING:
			return &"ACT_CROUCHING"
		LibSM64.ACT_START_CROUCHING:
			return &"ACT_START_CROUCHING"
		LibSM64.ACT_STOP_CROUCHING:
			return &"ACT_STOP_CROUCHING"
		LibSM64.ACT_START_CRAWLING:
			return &"ACT_START_CRAWLING"
		LibSM64.ACT_STOP_CRAWLING:
			return &"ACT_STOP_CRAWLING"
		LibSM64.ACT_SLIDE_KICK_SLIDE_STOP:
			return &"ACT_SLIDE_KICK_SLIDE_STOP"
		LibSM64.ACT_SHOCKWAVE_BOUNCE:
			return &"ACT_SHOCKWAVE_BOUNCE"
		LibSM64.ACT_FIRST_PERSON:
			return &"ACT_FIRST_PERSON"
		LibSM64.ACT_BACKFLIP_LAND_STOP:
			return &"ACT_BACKFLIP_LAND_STOP"
		LibSM64.ACT_JUMP_LAND_STOP:
			return &"ACT_JUMP_LAND_STOP"
		LibSM64.ACT_DOUBLE_JUMP_LAND_STOP:
			return &"ACT_DOUBLE_JUMP_LAND_STOP"
		LibSM64.ACT_FREEFALL_LAND_STOP:
			return &"ACT_FREEFALL_LAND_STOP"
		LibSM64.ACT_SIDE_FLIP_LAND_STOP:
			return &"ACT_SIDE_FLIP_LAND_STOP"
		LibSM64.ACT_HOLD_JUMP_LAND_STOP:
			return &"ACT_HOLD_JUMP_LAND_STOP"
		LibSM64.ACT_HOLD_FREEFALL_LAND_STOP:
			return &"ACT_HOLD_FREEFALL_LAND_STOP"
		LibSM64.ACT_AIR_THROW_LAND:
			return &"ACT_AIR_THROW_LAND"
		LibSM64.ACT_TWIRL_LAND:
			return &"ACT_TWIRL_LAND"
		LibSM64.ACT_LAVA_BOOST_LAND:
			return &"ACT_LAVA_BOOST_LAND"
		LibSM64.ACT_TRIPLE_JUMP_LAND_STOP:
			return &"ACT_TRIPLE_JUMP_LAND_STOP"
		LibSM64.ACT_LONG_JUMP_LAND_STOP:
			return &"ACT_LONG_JUMP_LAND_STOP"
		LibSM64.ACT_GROUND_POUND_LAND:
			return &"ACT_GROUND_POUND_LAND"
		LibSM64.ACT_BRAKING_STOP:
			return &"ACT_BRAKING_STOP"
		LibSM64.ACT_BUTT_SLIDE_STOP:
			return &"ACT_BUTT_SLIDE_STOP"
		LibSM64.ACT_HOLD_BUTT_SLIDE_STOP:
			return &"ACT_HOLD_BUTT_SLIDE_STOP"
		LibSM64.ACT_WALKING:
			return &"ACT_WALKING"
		LibSM64.ACT_HOLD_WALKING:
			return &"ACT_HOLD_WALKING"
		LibSM64.ACT_TURNING_AROUND:
			return &"ACT_TURNING_AROUND"
		LibSM64.ACT_FINISH_TURNING_AROUND:
			return &"ACT_FINISH_TURNING_AROUND"
		LibSM64.ACT_BRAKING:
			return &"ACT_BRAKING"
		LibSM64.ACT_RIDING_SHELL_GROUND:
			return &"ACT_RIDING_SHELL_GROUND"
		LibSM64.ACT_HOLD_HEAVY_WALKING:
			return &"ACT_HOLD_HEAVY_WALKING"
		LibSM64.ACT_CRAWLING:
			return &"ACT_CRAWLING"
		LibSM64.ACT_BURNING_GROUND:
			return &"ACT_BURNING_GROUND"
		LibSM64.ACT_DECELERATING:
			return &"ACT_DECELERATING"
		LibSM64.ACT_HOLD_DECELERATING:
			return &"ACT_HOLD_DECELERATING"
		LibSM64.ACT_BEGIN_SLIDING:
			return &"ACT_BEGIN_SLIDING"
		LibSM64.ACT_HOLD_BEGIN_SLIDING:
			return &"ACT_HOLD_BEGIN_SLIDING"
		LibSM64.ACT_BUTT_SLIDE:
			return &"ACT_BUTT_SLIDE"
		LibSM64.ACT_STOMACH_SLIDE:
			return &"ACT_STOMACH_SLIDE"
		LibSM64.ACT_HOLD_BUTT_SLIDE:
			return &"ACT_HOLD_BUTT_SLIDE"
		LibSM64.ACT_HOLD_STOMACH_SLIDE:
			return &"ACT_HOLD_STOMACH_SLIDE"
		LibSM64.ACT_DIVE_SLIDE:
			return &"ACT_DIVE_SLIDE"
		LibSM64.ACT_MOVE_PUNCHING:
			return &"ACT_MOVE_PUNCHING"
		LibSM64.ACT_CROUCH_SLIDE:
			return &"ACT_CROUCH_SLIDE"
		LibSM64.ACT_SLIDE_KICK_SLIDE:
			return &"ACT_SLIDE_KICK_SLIDE"
		LibSM64.ACT_HARD_BACKWARD_GROUND_KB:
			return &"ACT_HARD_BACKWARD_GROUND_KB"
		LibSM64.ACT_HARD_FORWARD_GROUND_KB:
			return &"ACT_HARD_FORWARD_GROUND_KB"
		LibSM64.ACT_BACKWARD_GROUND_KB:
			return &"ACT_BACKWARD_GROUND_KB"
		LibSM64.ACT_FORWARD_GROUND_KB:
			return &"ACT_FORWARD_GROUND_KB"
		LibSM64.ACT_SOFT_BACKWARD_GROUND_KB:
			return &"ACT_SOFT_BACKWARD_GROUND_KB"
		LibSM64.ACT_SOFT_FORWARD_GROUND_KB:
			return &"ACT_SOFT_FORWARD_GROUND_KB"
		LibSM64.ACT_GROUND_BONK:
			return &"ACT_GROUND_BONK"
		LibSM64.ACT_DEATH_EXIT_LAND:
			return &"ACT_DEATH_EXIT_LAND"
		LibSM64.ACT_JUMP_LAND:
			return &"ACT_JUMP_LAND"
		LibSM64.ACT_FREEFALL_LAND:
			return &"ACT_FREEFALL_LAND"
		LibSM64.ACT_DOUBLE_JUMP_LAND:
			return &"ACT_DOUBLE_JUMP_LAND"
		LibSM64.ACT_SIDE_FLIP_LAND:
			return &"ACT_SIDE_FLIP_LAND"
		LibSM64.ACT_HOLD_JUMP_LAND:
			return &"ACT_HOLD_JUMP_LAND"
		LibSM64.ACT_HOLD_FREEFALL_LAND:
			return &"ACT_HOLD_FREEFALL_LAND"
		LibSM64.ACT_QUICKSAND_JUMP_LAND:
			return &"ACT_QUICKSAND_JUMP_LAND"
		LibSM64.ACT_HOLD_QUICKSAND_JUMP_LAND:
			return &"ACT_HOLD_QUICKSAND_JUMP_LAND"
		LibSM64.ACT_TRIPLE_JUMP_LAND:
			return &"ACT_TRIPLE_JUMP_LAND"
		LibSM64.ACT_LONG_JUMP_LAND:
			return &"ACT_LONG_JUMP_LAND"
		LibSM64.ACT_BACKFLIP_LAND:
			return &"ACT_BACKFLIP_LAND"
		LibSM64.ACT_JUMP:
			return &"ACT_JUMP"
		LibSM64.ACT_DOUBLE_JUMP:
			return &"ACT_DOUBLE_JUMP"
		LibSM64.ACT_TRIPLE_JUMP:
			return &"ACT_TRIPLE_JUMP"
		LibSM64.ACT_BACKFLIP:
			return &"ACT_BACKFLIP"
		LibSM64.ACT_STEEP_JUMP:
			return &"ACT_STEEP_JUMP"
		LibSM64.ACT_WALL_KICK_AIR:
			return &"ACT_WALL_KICK_AIR"
		LibSM64.ACT_SIDE_FLIP:
			return &"ACT_SIDE_FLIP"
		LibSM64.ACT_LONG_JUMP:
			return &"ACT_LONG_JUMP"
		LibSM64.ACT_WATER_JUMP:
			return &"ACT_WATER_JUMP"
		LibSM64.ACT_DIVE:
			return &"ACT_DIVE"
		LibSM64.ACT_FREEFALL:
			return &"ACT_FREEFALL"
		LibSM64.ACT_TOP_OF_POLE_JUMP:
			return &"ACT_TOP_OF_POLE_JUMP"
		LibSM64.ACT_BUTT_SLIDE_AIR:
			return &"ACT_BUTT_SLIDE_AIR"
		LibSM64.ACT_FLYING_TRIPLE_JUMP:
			return &"ACT_FLYING_TRIPLE_JUMP"
		LibSM64.ACT_SHOT_FROM_CANNON:
			return &"ACT_SHOT_FROM_CANNON"
		LibSM64.ACT_FLYING:
			return &"ACT_FLYING"
		LibSM64.ACT_RIDING_SHELL_JUMP:
			return &"ACT_RIDING_SHELL_JUMP"
		LibSM64.ACT_RIDING_SHELL_FALL:
			return &"ACT_RIDING_SHELL_FALL"
		LibSM64.ACT_VERTICAL_WIND:
			return &"ACT_VERTICAL_WIND"
		LibSM64.ACT_HOLD_JUMP:
			return &"ACT_HOLD_JUMP"
		LibSM64.ACT_HOLD_FREEFALL:
			return &"ACT_HOLD_FREEFALL"
		LibSM64.ACT_HOLD_BUTT_SLIDE_AIR:
			return &"ACT_HOLD_BUTT_SLIDE_AIR"
		LibSM64.ACT_HOLD_WATER_JUMP:
			return &"ACT_HOLD_WATER_JUMP"
		LibSM64.ACT_TWIRLING:
			return &"ACT_TWIRLING"
		LibSM64.ACT_FORWARD_ROLLOUT:
			return &"ACT_FORWARD_ROLLOUT"
		LibSM64.ACT_AIR_HIT_WALL:
			return &"ACT_AIR_HIT_WALL"
		LibSM64.ACT_RIDING_HOOT:
			return &"ACT_RIDING_HOOT"
		LibSM64.ACT_GROUND_POUND:
			return &"ACT_GROUND_POUND"
		LibSM64.ACT_SLIDE_KICK:
			return &"ACT_SLIDE_KICK"
		LibSM64.ACT_AIR_THROW:
			return &"ACT_AIR_THROW"
		LibSM64.ACT_JUMP_KICK:
			return &"ACT_JUMP_KICK"
		LibSM64.ACT_BACKWARD_ROLLOUT:
			return &"ACT_BACKWARD_ROLLOUT"
		LibSM64.ACT_CRAZY_BOX_BOUNCE:
			return &"ACT_CRAZY_BOX_BOUNCE"
		LibSM64.ACT_SPECIAL_TRIPLE_JUMP:
			return &"ACT_SPECIAL_TRIPLE_JUMP"
		LibSM64.ACT_BACKWARD_AIR_KB:
			return &"ACT_BACKWARD_AIR_KB"
		LibSM64.ACT_FORWARD_AIR_KB:
			return &"ACT_FORWARD_AIR_KB"
		LibSM64.ACT_HARD_FORWARD_AIR_KB:
			return &"ACT_HARD_FORWARD_AIR_KB"
		LibSM64.ACT_HARD_BACKWARD_AIR_KB:
			return &"ACT_HARD_BACKWARD_AIR_KB"
		LibSM64.ACT_BURNING_JUMP:
			return &"ACT_BURNING_JUMP"
		LibSM64.ACT_BURNING_FALL:
			return &"ACT_BURNING_FALL"
		LibSM64.ACT_SOFT_BONK:
			return &"ACT_SOFT_BONK"
		LibSM64.ACT_LAVA_BOOST:
			return &"ACT_LAVA_BOOST"
		LibSM64.ACT_GETTING_BLOWN:
			return &"ACT_GETTING_BLOWN"
		LibSM64.ACT_THROWN_FORWARD:
			return &"ACT_THROWN_FORWARD"
		LibSM64.ACT_THROWN_BACKWARD:
			return &"ACT_THROWN_BACKWARD"
		LibSM64.ACT_WATER_IDLE:
			return &"ACT_WATER_IDLE"
		LibSM64.ACT_HOLD_WATER_IDLE:
			return &"ACT_HOLD_WATER_IDLE"
		LibSM64.ACT_WATER_ACTION_END:
			return &"ACT_WATER_ACTION_END"
		LibSM64.ACT_HOLD_WATER_ACTION_END:
			return &"ACT_HOLD_WATER_ACTION_END"
		LibSM64.ACT_DROWNING:
			return &"ACT_DROWNING"
		LibSM64.ACT_BACKWARD_WATER_KB:
			return &"ACT_BACKWARD_WATER_KB"
		LibSM64.ACT_FORWARD_WATER_KB:
			return &"ACT_FORWARD_WATER_KB"
		LibSM64.ACT_WATER_DEATH:
			return &"ACT_WATER_DEATH"
		LibSM64.ACT_WATER_SHOCKED:
			return &"ACT_WATER_SHOCKED"
		LibSM64.ACT_BREASTSTROKE:
			return &"ACT_BREASTSTROKE"
		LibSM64.ACT_SWIMMING_END:
			return &"ACT_SWIMMING_END"
		LibSM64.ACT_FLUTTER_KICK:
			return &"ACT_FLUTTER_KICK"
		LibSM64.ACT_HOLD_BREASTSTROKE:
			return &"ACT_HOLD_BREASTSTROKE"
		LibSM64.ACT_HOLD_SWIMMING_END:
			return &"ACT_HOLD_SWIMMING_END"
		LibSM64.ACT_HOLD_FLUTTER_KICK:
			return &"ACT_HOLD_FLUTTER_KICK"
		LibSM64.ACT_WATER_SHELL_SWIMMING:
			return &"ACT_WATER_SHELL_SWIMMING"
		LibSM64.ACT_WATER_THROW:
			return &"ACT_WATER_THROW"
		LibSM64.ACT_WATER_PUNCH:
			return &"ACT_WATER_PUNCH"
		LibSM64.ACT_WATER_PLUNGE:
			return &"ACT_WATER_PLUNGE"
		LibSM64.ACT_CAUGHT_IN_WHIRLPOOL:
			return &"ACT_CAUGHT_IN_WHIRLPOOL"
		LibSM64.ACT_METAL_WATER_STANDING:
			return &"ACT_METAL_WATER_STANDING"
		LibSM64.ACT_HOLD_METAL_WATER_STANDING:
			return &"ACT_HOLD_METAL_WATER_STANDING"
		LibSM64.ACT_METAL_WATER_WALKING:
			return &"ACT_METAL_WATER_WALKING"
		LibSM64.ACT_HOLD_METAL_WATER_WALKING:
			return &"ACT_HOLD_METAL_WATER_WALKING"
		LibSM64.ACT_METAL_WATER_FALLING:
			return &"ACT_METAL_WATER_FALLING"
		LibSM64.ACT_HOLD_METAL_WATER_FALLING:
			return &"ACT_HOLD_METAL_WATER_FALLING"
		LibSM64.ACT_METAL_WATER_FALL_LAND:
			return &"ACT_METAL_WATER_FALL_LAND"
		LibSM64.ACT_HOLD_METAL_WATER_FALL_LAND:
			return &"ACT_HOLD_METAL_WATER_FALL_LAND"
		LibSM64.ACT_METAL_WATER_JUMP:
			return &"ACT_METAL_WATER_JUMP"
		LibSM64.ACT_HOLD_METAL_WATER_JUMP:
			return &"ACT_HOLD_METAL_WATER_JUMP"
		LibSM64.ACT_METAL_WATER_JUMP_LAND:
			return &"ACT_METAL_WATER_JUMP_LAND"
		LibSM64.ACT_HOLD_METAL_WATER_JUMP_LAND:
			return &"ACT_HOLD_METAL_WATER_JUMP_LAND"
		LibSM64.ACT_DISAPPEARED:
			return &"ACT_DISAPPEARED"
		LibSM64.ACT_INTRO_CUTSCENE:
			return &"ACT_INTRO_CUTSCENE"
		LibSM64.ACT_STAR_DANCE_EXIT:
			return &"ACT_STAR_DANCE_EXIT"
		LibSM64.ACT_STAR_DANCE_WATER:
			return &"ACT_STAR_DANCE_WATER"
		LibSM64.ACT_FALL_AFTER_STAR_GRAB:
			return &"ACT_FALL_AFTER_STAR_GRAB"
		LibSM64.ACT_READING_AUTOMATIC_DIALOG:
			return &"ACT_READING_AUTOMATIC_DIALOG"
		LibSM64.ACT_READING_NPC_DIALOG:
			return &"ACT_READING_NPC_DIALOG"
		LibSM64.ACT_STAR_DANCE_NO_EXIT:
			return &"ACT_STAR_DANCE_NO_EXIT"
		LibSM64.ACT_READING_SIGN:
			return &"ACT_READING_SIGN"
		LibSM64.ACT_JUMBO_STAR_CUTSCENE:
			return &"ACT_JUMBO_STAR_CUTSCENE"
		LibSM64.ACT_WAITING_FOR_DIALOG:
			return &"ACT_WAITING_FOR_DIALOG"
		LibSM64.ACT_DEBUG_FREE_MOVE:
			return &"ACT_DEBUG_FREE_MOVE"
		LibSM64.ACT_STANDING_DEATH:
			return &"ACT_STANDING_DEATH"
		LibSM64.ACT_QUICKSAND_DEATH:
			return &"ACT_QUICKSAND_DEATH"
		LibSM64.ACT_ELECTROCUTION:
			return &"ACT_ELECTROCUTION"
		LibSM64.ACT_SUFFOCATION:
			return &"ACT_SUFFOCATION"
		LibSM64.ACT_DEATH_ON_STOMACH:
			return &"ACT_DEATH_ON_STOMACH"
		LibSM64.ACT_DEATH_ON_BACK:
			return &"ACT_DEATH_ON_BACK"
		LibSM64.ACT_EATEN_BY_BUBBA:
			return &"ACT_EATEN_BY_BUBBA"
		LibSM64.ACT_END_PEACH_CUTSCENE:
			return &"ACT_END_PEACH_CUTSCENE"
		LibSM64.ACT_CREDITS_CUTSCENE:
			return &"ACT_CREDITS_CUTSCENE"
		LibSM64.ACT_END_WAVING_CUTSCENE:
			return &"ACT_END_WAVING_CUTSCENE"
		LibSM64.ACT_PULLING_DOOR:
			return &"ACT_PULLING_DOOR"
		LibSM64.ACT_PUSHING_DOOR:
			return &"ACT_PUSHING_DOOR"
		LibSM64.ACT_WARP_DOOR_SPAWN:
			return &"ACT_WARP_DOOR_SPAWN"
		LibSM64.ACT_EMERGE_FROM_PIPE:
			return &"ACT_EMERGE_FROM_PIPE"
		LibSM64.ACT_SPAWN_SPIN_AIRBORNE:
			return &"ACT_SPAWN_SPIN_AIRBORNE"
		LibSM64.ACT_SPAWN_SPIN_LANDING:
			return &"ACT_SPAWN_SPIN_LANDING"
		LibSM64.ACT_EXIT_AIRBORNE:
			return &"ACT_EXIT_AIRBORNE"
		LibSM64.ACT_EXIT_LAND_SAVE_DIALOG:
			return &"ACT_EXIT_LAND_SAVE_DIALOG"
		LibSM64.ACT_DEATH_EXIT:
			return &"ACT_DEATH_EXIT"
		LibSM64.ACT_UNUSED_DEATH_EXIT:
			return &"ACT_UNUSED_DEATH_EXIT"
		LibSM64.ACT_FALLING_DEATH_EXIT:
			return &"ACT_FALLING_DEATH_EXIT"
		LibSM64.ACT_SPECIAL_EXIT_AIRBORNE:
			return &"ACT_SPECIAL_EXIT_AIRBORNE"
		LibSM64.ACT_SPECIAL_DEATH_EXIT:
			return &"ACT_SPECIAL_DEATH_EXIT"
		LibSM64.ACT_FALLING_EXIT_AIRBORNE:
			return &"ACT_FALLING_EXIT_AIRBORNE"
		LibSM64.ACT_UNLOCKING_KEY_DOOR:
			return &"ACT_UNLOCKING_KEY_DOOR"
		LibSM64.ACT_UNLOCKING_STAR_DOOR:
			return &"ACT_UNLOCKING_STAR_DOOR"
		LibSM64.ACT_ENTERING_STAR_DOOR:
			return &"ACT_ENTERING_STAR_DOOR"
		LibSM64.ACT_SPAWN_NO_SPIN_AIRBORNE:
			return &"ACT_SPAWN_NO_SPIN_AIRBORNE"
		LibSM64.ACT_SPAWN_NO_SPIN_LANDING:
			return &"ACT_SPAWN_NO_SPIN_LANDING"
		LibSM64.ACT_BBH_ENTER_JUMP:
			return &"ACT_BBH_ENTER_JUMP"
		LibSM64.ACT_BBH_ENTER_SPIN:
			return &"ACT_BBH_ENTER_SPIN"
		LibSM64.ACT_TELEPORT_FADE_OUT:
			return &"ACT_TELEPORT_FADE_OUT"
		LibSM64.ACT_TELEPORT_FADE_IN:
			return &"ACT_TELEPORT_FADE_IN"
		LibSM64.ACT_SHOCKED:
			return &"ACT_SHOCKED"
		LibSM64.ACT_SQUISHED:
			return &"ACT_SQUISHED"
		LibSM64.ACT_HEAD_STUCK_IN_GROUND:
			return &"ACT_HEAD_STUCK_IN_GROUND"
		LibSM64.ACT_BUTT_STUCK_IN_GROUND:
			return &"ACT_BUTT_STUCK_IN_GROUND"
		LibSM64.ACT_FEET_STUCK_IN_GROUND:
			return &"ACT_FEET_STUCK_IN_GROUND"
		LibSM64.ACT_PUTTING_ON_CAP:
			return &"ACT_PUTTING_ON_CAP"
		LibSM64.ACT_HOLDING_POLE:
			return &"ACT_HOLDING_POLE"
		LibSM64.ACT_GRAB_POLE_SLOW:
			return &"ACT_GRAB_POLE_SLOW"
		LibSM64.ACT_GRAB_POLE_FAST:
			return &"ACT_GRAB_POLE_FAST"
		LibSM64.ACT_CLIMBING_POLE:
			return &"ACT_CLIMBING_POLE"
		LibSM64.ACT_TOP_OF_POLE_TRANSITION:
			return &"ACT_TOP_OF_POLE_TRANSITION"
		LibSM64.ACT_TOP_OF_POLE:
			return &"ACT_TOP_OF_POLE"
		LibSM64.ACT_START_HANGING:
			return &"ACT_START_HANGING"
		LibSM64.ACT_HANGING:
			return &"ACT_HANGING"
		LibSM64.ACT_HANG_MOVING:
			return &"ACT_HANG_MOVING"
		LibSM64.ACT_LEDGE_GRAB:
			return &"ACT_LEDGE_GRAB"
		LibSM64.ACT_LEDGE_CLIMB_SLOW_1:
			return &"ACT_LEDGE_CLIMB_SLOW_1"
		LibSM64.ACT_LEDGE_CLIMB_SLOW_2:
			return &"ACT_LEDGE_CLIMB_SLOW_2"
		LibSM64.ACT_LEDGE_CLIMB_DOWN:
			return &"ACT_LEDGE_CLIMB_DOWN"
		LibSM64.ACT_LEDGE_CLIMB_FAST:
			return &"ACT_LEDGE_CLIMB_FAST"
		LibSM64.ACT_GRABBED:
			return &"ACT_GRABBED"
		LibSM64.ACT_IN_CANNON:
			return &"ACT_IN_CANNON"
		LibSM64.ACT_TORNADO_TWIRLING:
			return &"ACT_TORNADO_TWIRLING"
		LibSM64.ACT_PUNCHING:
			return &"ACT_PUNCHING"
		LibSM64.ACT_PICKING_UP:
			return &"ACT_PICKING_UP"
		LibSM64.ACT_DIVE_PICKING_UP:
			return &"ACT_DIVE_PICKING_UP"
		LibSM64.ACT_STOMACH_SLIDE_STOP:
			return &"ACT_STOMACH_SLIDE_STOP"
		LibSM64.ACT_PLACING_DOWN:
			return &"ACT_PLACING_DOWN"
		LibSM64.ACT_THROWING:
			return &"ACT_THROWING"
		LibSM64.ACT_HEAVY_THROW:
			return &"ACT_HEAVY_THROW"
		LibSM64.ACT_PICKING_UP_BOWSER:
			return &"ACT_PICKING_UP_BOWSER"
		LibSM64.ACT_HOLDING_BOWSER:
			return &"ACT_HOLDING_BOWSER"
		LibSM64.ACT_RELEASING_BOWSER:
			return &"ACT_RELEASING_BOWSER"
		_:
			return &"ACT_UNKNOWN"
