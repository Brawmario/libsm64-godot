class_name LibSM64StaticSurfacesHandler
extends LibSM64SurfaceHandlerBase

## Node that handles adding nodes as Static Surfaces for libsm64.


## Group name that contains the nodes that are part of the scene's static surfaces
@export var static_surfaces_group := &"libsm64_static_surfaces"


## Load all nodes in the [member static_surfaces_group] as static surfaces in the [code]libsm64[/code] world. This function should be called before any calls to [method LibSM64.mario_create].
## [b]Note:[/b] calling this function multiple times will overwrite the previous calls.
## [b]Warning:[/b] These surfaces will delimit the bounderies of the [code]libsm64[/code] world in the XZ plane. This means that Mario can only be above theses surfaces and will hit "invisible walls" at the edges of the world.
func load_static_surfaces() -> void:
	var libsm64_surface_array := LibSM64SurfaceArray.new()

	for node in get_tree().get_nodes_in_group(static_surfaces_group):
		var node_3d := node as Node3D
		if not node_3d:
			push_error("Non Node3D %s in %s group" % [node, static_surfaces_group])
			continue

		var properties := find_surface_properties(node_3d)
		var faces := get_faces_from_node(node_3d)
		if faces.is_empty():
			push_error("Faces array empty, skipping node %s" % node_3d.name)
			continue

		for i in range(0, faces.size(), 3):
			var v1 := node_3d.global_transform * faces[i + 0]
			var v2 := node_3d.global_transform * faces[i + 1]
			var v3 := node_3d.global_transform * faces[i + 2]
			if properties:
				libsm64_surface_array.add_triangle_with_properties(v1, v2, v3, properties)
			else:
				libsm64_surface_array.add_triangle(v1, v2, v3)

	LibSM64.static_surfaces_load(libsm64_surface_array)
