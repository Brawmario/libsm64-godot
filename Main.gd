extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

var sm64_handler

# Called when the node enters the scene tree for the first time.
func _ready():
	sm64_handler = load("res://SM64Handler.gdns").new()
	sm64_handler.global_init("baserom.us.z64")


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
