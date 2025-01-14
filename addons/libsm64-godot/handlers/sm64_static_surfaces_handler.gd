class_name SM64StaticSurfaceHandler
extends Node

## Node that handles adding MeshInstance3D nodes as Static Surfaces for libsm64.

## Group name that contains the MeshInstance3D that are part of the scene's static surfaces
@export var static_surfaces_group := &"libsm64_static_surfaces"

var _default_surface_properties := SM64SurfaceProperties.new()


## Load all MeshInstance3D in static_surfaces_group into SM64
## [b]Warning:[/b] there should be at least one plane that spans the entire playing area at the bottom of the map.
func load_static_surfaces() -> void:
	var surface_properties_array: Array[SM64SurfaceProperties] = []

	var libsm64_surface_array := LibSM64SurfaceArray.new()

	for node in get_tree().get_nodes_in_group(static_surfaces_group):
		var mesh_instance := node as MeshInstance3D
		if not mesh_instance:
			push_warning("Non MeshInstance3D in %s group" % static_surfaces_group)
			continue

		var properties := _find_surface_properties(mesh_instance)
		var mesh_faces := mesh_instance.get_mesh().get_faces()
		for i in range(0, mesh_faces.size(), 3):
			var v1 = mesh_instance.global_transform * mesh_faces[i + 0]
			var v2 = mesh_instance.global_transform * mesh_faces[i + 1]
			var v3 = mesh_instance.global_transform * mesh_faces[i + 2]
			libsm64_surface_array.add_triangle(v1, v2, v3, properties.surface_type, properties.terrain_type, properties.force)

	LibSM64.static_surfaces_load(libsm64_surface_array)


func _find_surface_properties(node: Node) -> SM64SurfaceProperties:
	for child in node.get_children():
		if child is SM64SurfacePropertiesComponent:
			return child.surface_properties

	return _default_surface_properties
