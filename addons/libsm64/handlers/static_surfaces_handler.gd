class_name StaticSurfaceHandler
extends Node


## SM64Handler instance
@export var sm64_handler: SM64Handler
## Group name that contains the MeshInstance3D that are part of the scene's static surfaces
@export var static_surfaces_group := "libsm64_static_surfaces"


## Load all MeshInstance3D in static_surfaces_group into the SM64Handler instance
func load_static_sufaces() -> void:
	var faces := PackedVector3Array()

	for node in get_tree().get_nodes_in_group(static_surfaces_group):
		var mesh_instance := node as MeshInstance3D
		if not mesh_instance:
			push_warning("Non MeshInstance3D in %s group" % static_surfaces_group)
			continue
		var mesh_faces := mesh_instance.get_mesh().get_faces()
		var offset := mesh_instance.global_transform.origin
		for i in range(mesh_faces.size()):
			mesh_faces[i] += offset
		faces.append_array(mesh_faces)

	sm64_handler.static_surfaces_load(faces)
