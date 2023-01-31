extends Node3D


func _ready() -> void:
	LibSM64.global_init()
	$SM64StaticSurfacesHandler.load_static_surfaces()
	$SM64SurfaceObjectsHandler.load_all_surface_objects()

	for node in get_tree().get_nodes_in_group("libsm64_mario"):
		var mario := node as SM64Mario
		if not mario:
			continue
		mario.create()
		mario.water_level = 0.0


func _on_initial_scenario_tree_exiting() -> void:
	for node in get_tree().get_nodes_in_group("libsm64_mario"):
		var mario := node as SM64Mario
		if not mario:
			continue
		mario.delete()
	LibSM64.global_terminate()
