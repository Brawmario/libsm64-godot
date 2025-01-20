extends PanelContainer


const MARIO_INFO_FORMAT := """\
Action: %s
Health wedges: %d
Invicibility Time: %.2f
Face angle: %dº
"""

var mario: LibSM64Mario

@onready var mario_info_label := %MarioInfoLabel as Label


func _process(_delta: float) -> void:
	if mario:
		mario_info_label.text = MARIO_INFO_FORMAT % [
				mario.action_name,
				mario.health_wedges,
				mario.invicibility_time,
				rad_to_deg(mario.face_angle)]


func _on_return_button_pressed() -> void:
	get_tree().root.get_child(-1).return_to_menu()
