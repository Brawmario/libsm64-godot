class_name LibSM64SurfaceObjectsHandler
extends LibSM64SurfaceHandlerBase

## Node that handles adding and updating nodes as Surface Objects for libsm64.


## Group name that contains the nodes that are part of the scene's surface objects.
@export var surface_objects_group := &"libsm64_surface_objects"


var _surface_objects_ids: Array[int] = []
var _surface_objects_refs: Array[Node3D] = []
var _time_since_last_tick := 0.0


func _physics_process(delta: float) -> void:
	_time_since_last_tick += delta
	if _time_since_last_tick >= LibSM64.tick_delta_time:
		_update_surface_objects()
		_time_since_last_tick = 0.0


func _update_surface_objects() -> void:
	for i in range(_surface_objects_ids.size()):
		var id := _surface_objects_ids[i]
		var transform := _surface_objects_refs[i].global_transform
		var position := transform.origin
		var rotation := transform.basis.get_rotation_quaternion()
		LibSM64.surface_object_move(id, position, rotation)


## Load a node as a surface object in the [code]libsm64[/code] world.
func load_surface_object(node: Node3D) -> void:
	var libsm64_surface_array := LibSM64SurfaceArray.new()

	var faces := get_faces_from_node(node)
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

	_surface_objects_ids.push_back(surface_object_id)
	_surface_objects_refs.push_back(node)

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
	var index := _surface_objects_refs.find(node_3d)
	if index == -1:
		return

	var id := _surface_objects_ids[index]
	LibSM64.surface_object_delete(id)
	_surface_objects_refs.remove_at(index)
	_surface_objects_ids.remove_at(index)


## Delete all surface objects from the [code]libsm64[/code] world.
func delete_all_surface_objects() -> void:
	for id in _surface_objects_ids:
		LibSM64.surface_object_delete(id)

	_surface_objects_refs.clear()
	_surface_objects_ids.clear()
