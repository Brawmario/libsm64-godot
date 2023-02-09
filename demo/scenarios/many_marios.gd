extends Node3D


func _ready() -> void:
	SM64Global.scale_factor = 75
	SM64Global.init()
	$SM64StaticSurfacesHandler.load_static_surfaces()
	$SM64SurfaceObjectsHandler.load_all_surface_objects()

	for node in get_tree().get_nodes_in_group("libsm64_mario"):
		var mario := node as SM64Mario
		if not mario:
			continue
		mario.create()
		mario.water_level = 0.0


func _on_tree_exiting() -> void:
	for node in get_tree().get_nodes_in_group("libsm64_mario"):
		var mario := node as SM64Mario
		if not mario:
			continue
		mario.delete()
	SM64Global.terminate()
