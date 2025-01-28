class_name LibSM64SurfaceObjectsHandler
extends LibSM64SurfaceHandlerBase

## Node that handles adding and updating nodes as Surface Objects for libsm64.


## Group name that contains the nodes that are part of the scene's surface objects.
@export var surface_objects_group := &"libsm64_surface_objects"


var _surface_objects_map := {}
var _time_since_last_tick := 0.0


func _physics_process(delta: float) -> void:
	_time_since_last_tick += delta
	if _time_since_last_tick >= LibSM64.tick_delta_time:
		_update_surface_objects()
		# We don't want to update the surface objects more than once per tick.
		_time_since_last_tick = 0.0


func _update_surface_objects() -> void:
	for node in _surface_objects_map:
		var id := _surface_objects_map[node] as int
		var transform := node.global_transform as Transform3D
		var position := transform.origin
		var rotation := transform.basis.get_rotation_quaternion()
		LibSM64.surface_object_move(id, position, rotation)


## Load a node as a surface object in the [code]libsm64[/code] world.
func load_surface_object(node: Node3D) -> void:
	var libsm64_surface_array := LibSM64SurfaceArray.new()

	var faces := get_faces_from_node(node)
	if faces.is_empty():
		push_error("Node %s has no faces." % node.name)
		return

	var transform := node.global_transform
	var position := transform.origin
	var rotation := transform.basis.get_rotation_quaternion()
	var properties := find_surface_properties(node)

	for i in range(0, faces.size(), 3):
		var v1 := faces[i + 0]
		var v2 := faces[i + 1]
		var v3 := faces[i + 2]
		if properties:
			libsm64_surface_array.add_triangle_with_properties(v1, v2, v3, properties)
		else:
			libsm64_surface_array.add_triangle(v1, v2, v3)

	var surface_object_id := LibSM64.surface_object_create(position, rotation, libsm64_surface_array)

	_surface_objects_map[node] = surface_object_id

	# Clean up automaticaly if [Node3D] is removed from tree or freed
	node.tree_exiting.connect(delete_surface_object.bind(node), CONNECT_ONE_SHOT)


## Load all nodes in the [member surface_objects_group] as surface objects in the [code]libsm64[/code] world.
func load_all_surface_objects() -> void:
	for node in get_tree().get_nodes_in_group(surface_objects_group):
		var node_3d := node as Node3D
		if not node_3d:
			push_warning("Non Node3D in %s group" % surface_objects_group)
			continue
		load_surface_object(node_3d)


## Delete a surface object created from the given [param node_3d] from the [code]libsm64[/code] world if present.
func delete_surface_object(node_3d: Node3D) -> void:
	var id := _surface_objects_map.get(node_3d, -1) as int
	if id == -1:
		push_error("Node %s not found in surface objects map" % node_3d.name)
		return

	LibSM64.surface_object_delete(id)
	_surface_objects_map.erase(node_3d)


## Delete all surface objects from the [code]libsm64[/code] world.
func delete_all_surface_objects() -> void:
	for id in _surface_objects_map.values():
		LibSM64.surface_object_delete(id)

	_surface_objects_map.clear()
