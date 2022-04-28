tool
extends EditorPlugin


func _enter_tree():
	add_custom_type("Mario", "Spatial", preload("mario/Mario.gd"), preload("mario/mario-godot.svg"))
	add_custom_type("SM64Handler", "Resource", preload("sm64_handler/SM64Handler.gdns"), null)
	add_custom_type("SurfaceProperties", "Resource", preload("SurfaceProperties.gd"), null)
	pass


func _exit_tree():
	remove_custom_type("Mario")
	remove_custom_type("SM64Handler")
	remove_custom_type("SurfaceProperties")
	pass
