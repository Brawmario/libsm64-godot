class_name SM64StaticSurfaceHandler
extends Node

## Node that handles adding MeshInstance3D nodes as Static Surfaces for libsm64.

## Group name that contains the MeshInstance3D that are part of the scene's static surfaces
@export var static_surfaces_group := &"libsm64_static_surfaces"

var _default_surface_properties := SM64SurfaceProperties.new()


## Load all MeshInstance3D in static_surfaces_group into SM64
func load_static_surfaces() -> void:
	var faces := PackedVector3Array()
	var surface_properties_array: Array[SM64SurfaceProperties] = []

	for node in get_tree().get_nodes_in_group(static_surfaces_group):
		var mesh_instance := node as MeshInstance3D
		if not mesh_instance:
			push_warning("Non MeshInstance3D in %s group" % static_surfaces_group)
			continue

		var mesh_faces := mesh_instance.get_mesh().get_faces()
		for i in range(mesh_faces.size()):
			mesh_faces[i] = mesh_instance.global_transform * mesh_faces[i]
		faces.append_array(mesh_faces)

		var surface_properties := _find_surface_properties(mesh_instance)
		var array: Array[SM64SurfaceProperties] = []
		array.resize(mesh_faces.size() / 3)
		array.fill(surface_properties)
		surface_properties_array.append_array(array)

	SM64Surfaces.static_surfaces_load(faces, surface_properties_array)


func _find_surface_properties(node: Node) -> SM64SurfaceProperties:
	for child in node.get_children():
		if child is SM64SurfacePropertiesComponent:
			return child.surface_properties

	return _default_surface_properties
