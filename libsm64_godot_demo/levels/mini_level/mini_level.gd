extends Node3D


func _ready() -> void:
	$Objects/MovingPlatform/AnimationPlayer.play("Move")
