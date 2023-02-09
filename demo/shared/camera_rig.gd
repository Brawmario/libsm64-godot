extends Node3D


const ANGLE_X_MIN := -PI / 4
const ANGLE_X_MAX := PI / 3

@export var is_y_inverted := false
@export var deadzone := PI / 10
@export var sensitivity_gamepad := Vector2(2.5, 2.5)
@export var sensitivity_mouse := Vector2(0.1, 0.1)

@onready var player := get_parent() as SM64Mario

var _input_relative := Vector2.ZERO


func _process(delta: float) -> void:
	global_transform.origin = player.global_transform.origin

	var look_direction := get_look_direction()
	var move_direction := get_move_direction()

	if _input_relative.length() > 0:
		update_rotation(_input_relative * sensitivity_mouse * delta)
		_input_relative = Vector2.ZERO
	elif look_direction.length() > 0:
		update_rotation(look_direction * sensitivity_gamepad * delta)

	rotation.y = wrapf(rotation.y, -PI, PI)


func _unhandled_input(event: InputEvent) -> void:
	var mouse_event := event as InputEventMouseMotion
	if mouse_event and Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		_input_relative += mouse_event.relative


func update_rotation(offset: Vector2) -> void:
	rotation.y -= offset.x
	rotation.x += offset.y * -1.0 if is_y_inverted else offset.y
	rotation.x = clamp(rotation.x, ANGLE_X_MIN, ANGLE_X_MAX)
	rotation.z = 0


# Returns the direction of the camera movement from the player
func get_look_direction() -> Vector2:
	return Vector2(Input.get_axis("camera_right", "camera_left"), Input.get_axis("camera_up", "camera_down")).normalized()


# Returns the move direction of the character controlled by the player
func get_move_direction() -> Vector3:
	return Vector3(
		Input.get_axis("mario_stick_right", "mario_stick_left"),
		0,
		Input.get_axis("mario_stick_down", "mario_stick_up")
	)
