extends Marker3D


@onready var _h_axis := $HorizontalAxis as Marker3D
@onready var _v_axis := $HorizontalAxis/VerticalAxis as Marker3D

var _h_accelaration := 0.1
var _v_accelaration := 0.1
var _stick_accelaration := 50.0

var _cam_rotation_h := 0.0
var _cam_rotation_v := 0.0


func _unhandled_input(event: InputEvent) -> void:
	_camera_event(event)
	_toggle_mouse_lock()


func _physics_process(delta: float) -> void:
	_camera_rotation(delta)


func _camera_rotation(delta: float) -> void:
	_cam_rotation_h += _stick_accelaration * Input.get_axis("camera_left", "camera_right")
	_cam_rotation_v += _stick_accelaration * Input.get_axis("camera_up", "camera_down")
	_h_axis.rotation.y = wrapf(_cam_rotation_h * _h_accelaration * delta, 0, 2 * PI)
	_v_axis.rotation.x = wrapf(_cam_rotation_v * _v_accelaration * delta, 0, 2 * PI)
	_cam_rotation_h = _h_axis.rotation.y / (_h_accelaration * delta)
	_cam_rotation_v = _v_axis.rotation.x / (_v_accelaration * delta)


func _camera_event(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		_cam_rotation_h += -event.relative.x
		_cam_rotation_v += -event.relative.y


func _toggle_mouse_lock() -> void:
	if Input.is_action_just_pressed("toggle_mouse_lock"):
		if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
			Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
		else:
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
