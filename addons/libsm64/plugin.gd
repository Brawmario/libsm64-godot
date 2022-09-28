@tool
extends EditorPlugin


func _enter_tree():
	add_custom_type("Mario", "Node3D", preload("mario/mario.gd"), preload("mario/mario-godot.svg"))
	add_custom_type("StaticSurfacesHandler", "Node", preload("handlers/static_surfaces_handler.gd"), null)
	add_custom_type("SurfaceObjectsHandler", "Node", preload("handlers/surface_objects_handler.gd"), null)


func _exit_tree():
	remove_custom_type("Mario")
	remove_custom_type("StaticSurfacesHandler")
	remove_custom_type("SurfaceObjectsHandler")
