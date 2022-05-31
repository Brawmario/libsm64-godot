extends Node

export var sm64_handler: Resource
export var static_surfaces_group := "libsm64_static_surfaces"


func load_static_sufaces() -> void:
	var faces := PoolVector3Array()

	for mesh_instance in get_tree().get_nodes_in_group(static_surfaces_group):
		var mesh_faces: PoolVector3Array = mesh_instance.get_mesh().get_faces()
		var offset: Vector3 = mesh_instance.global_transform.origin
		for i in range(mesh_faces.size()):
			mesh_faces[i] += offset
		faces.append_array(mesh_faces)

	sm64_handler.static_surfaces_load(faces)
