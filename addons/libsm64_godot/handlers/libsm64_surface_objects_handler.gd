class_name LibSM64SurfaceObjectsHandler
extends LibSM64SurfaceHandlerBase

## Node that handles adding and updating nodes as Surface Objects for libsm64.


## Group name that contains the nodes that are part of the scene's surface objects.
@export var surface_objects_group := &"libsm64_surface_objects"


var _surface_objects_map := {}
## The [code]tree_exiting[/code] [Callable] created for each node, so that it can be disconnected again.
var _surface_objects_callables := {}
var _time_since_last_tick := 0.0


func _physics_process(delta: float) -> void:
	_time_since_last_tick += delta
	if _time_since_last_tick >= LibSM64.tick_delta_time:
		_update_surface_objects()
		# We don't want to update the surface objects more than once per tick.
		_time_since_last_tick = 0.0


func _exit_tree() -> void:
	delete_all_surface_objects()


func _update_surface_objects() -> void:
	for node in _surface_objects_map.keys():
		if not is_instance_valid(node):
			# The node was freed while outside of the tree, so `tree_exiting` never fired for it.
			_forget_surface_object(node)
			continue

		var id := _surface_objects_map[node] as int
		var transform := node.global_transform as Transform3D
		var position := transform.origin
		var rotation := transform.basis.get_rotation_quaternion()
		LibSM64.surface_object_move(id, position, rotation)


## Load a node as a surface object in the [code]libsm64[/code] world.
func load_surface_object(node: Node3D) -> void:
	if _surface_objects_map.has(node):
		# Loading the same node twice would leak the surface object created by the previous call.
		delete_surface_object(node)

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
	var on_tree_exiting := delete_surface_object.bind(node)
	_surface_objects_callables[node] = on_tree_exiting
	node.tree_exiting.connect(on_tree_exiting, CONNECT_ONE_SHOT)


## Load all nodes in the [member surface_objects_group] as surface objects in the [code]libsm64[/code] world.
func load_all_surface_objects() -> void:
	for node in get_tree().get_nodes_in_group(surface_objects_group):
		var node_3d := node as Node3D
		if not node_3d:
			push_warning("Non Node3D in %s group" % surface_objects_group)
			continue
		load_surface_object(node_3d)


## Delete a surface object created from the given [param node_3d] from the [code]libsm64[/code] world if present.
## This is called automatically when [param node_3d] leaves the tree, and is safe to call more than once.
func delete_surface_object(node_3d: Node3D) -> void:
	var id := _surface_objects_map.get(node_3d, -1) as int
	if id == -1:
		# Already deleted (or never loaded), which is a valid sequence, not an error.
		return

	# The library may have been terminated before this node was freed.
	if LibSM64Global.is_initialized:
		LibSM64.surface_object_delete(id)

	_forget_surface_object(node_3d)


## Delete all surface objects from the [code]libsm64[/code] world.
func delete_all_surface_objects() -> void:
	for node in _surface_objects_map.keys():
		delete_surface_object(node)


## Remove the internal bookkeeping of [param node_3d], disconnecting its [code]tree_exiting[/code] handler.
func _forget_surface_object(node_3d: Node3D) -> void:
	_surface_objects_map.erase(node_3d)

	var on_tree_exiting: Callable = _surface_objects_callables.get(node_3d, Callable())
	_surface_objects_callables.erase(node_3d)
	if on_tree_exiting.is_valid() and is_instance_valid(node_3d) and node_3d.tree_exiting.is_connected(on_tree_exiting):
		node_3d.tree_exiting.disconnect(on_tree_exiting)
