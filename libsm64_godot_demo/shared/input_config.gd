extends Node


const InputEventKeyMap = {
	&"libsm64_mario_inputs_stick_left": KEY_A,
	&"libsm64_mario_inputs_stick_right": KEY_D,
	&"libsm64_mario_inputs_stick_up": KEY_W,
	&"libsm64_mario_inputs_stick_down": KEY_S,
	&"libsm64_mario_inputs_button_a": KEY_Z,
	&"libsm64_mario_inputs_button_b": KEY_X,
	&"libsm64_mario_inputs_button_z": KEY_SHIFT,
}

const InputEventJoypadMotionMap = {
	&"libsm64_mario_inputs_stick_left": [JOY_AXIS_LEFT_X, -1.0],
	&"libsm64_mario_inputs_stick_right": [JOY_AXIS_LEFT_X, 1.0],
	&"libsm64_mario_inputs_stick_up": [JOY_AXIS_LEFT_Y, -1.0],
	&"libsm64_mario_inputs_stick_down": [JOY_AXIS_LEFT_Y, 1.0],
	&"camera_left": [JOY_AXIS_RIGHT_X, -1.0],
	&"camera_right": [JOY_AXIS_RIGHT_X, 1.0],
	&"camera_up": [JOY_AXIS_RIGHT_Y, -1.0],
	&"camera_down": [JOY_AXIS_RIGHT_Y, 1.0],
}

const InputEventJoypadButtonMap = {
	&"libsm64_mario_inputs_button_a": JOY_BUTTON_A,
	&"libsm64_mario_inputs_button_b": JOY_BUTTON_X,
	&"libsm64_mario_inputs_button_z": JOY_BUTTON_LEFT_SHOULDER,
}


func _ready() -> void:
	for action in InputEventKeyMap:
		var physical_keycode = InputEventKeyMap[action]

		var input := InputEventKey.new()
		input.physical_keycode = physical_keycode

		if not InputMap.has_action(action):
			InputMap.add_action(action)
		InputMap.action_add_event(action, input)

	for action in InputEventJoypadMotionMap:
		var axis = InputEventJoypadMotionMap[action][0]
		var axis_value = InputEventJoypadMotionMap[action][1]

		var input := InputEventJoypadMotion.new()
		input.axis = axis
		input.axis_value = axis_value

		if not InputMap.has_action(action):
			InputMap.add_action(action)
		InputMap.action_add_event(action, input)

	for action in InputEventJoypadButtonMap:
		var button_index = InputEventJoypadButtonMap[action]

		var input := InputEventJoypadButton.new()
		input.button_index = button_index

		if not InputMap.has_action(action):
			InputMap.add_action(action)
		InputMap.action_add_event(action, input)

	# My job is done here, my planet needs me
	queue_free()
