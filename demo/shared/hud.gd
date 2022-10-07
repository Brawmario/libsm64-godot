extends PanelContainer


const MARIO_INFO_FORMAT := "Lives: %d
Health wedges: %d
Invicibility Time: %.2f
Face angle: %dº"

var mario: SM64Mario

@onready var mario_info_label := %MarioInfoLabel as Label


func _process(delta: float) -> void:
	if mario:
		mario_info_label.text = MARIO_INFO_FORMAT % [mario.lives, mario.health_wedges, mario.invicibility_time, rad_to_deg(mario.face_angle)]


func _on_return_button_pressed() -> void:
	get_tree().change_scene_to_packed(load("res://demo/main.tscn"))
