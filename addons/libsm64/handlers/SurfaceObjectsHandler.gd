extends Node

const FPS_30_DELTA = 1.0/30.0

@export var sm64_handler: Resource
@export var surface_objects_group := "libsm64_surface_objects"

var _surface_objects_ids := []
var _surface_objects_refs := []
var time_since_last_tick := 0.0


func _physics_process(delta: float) -> void:
	time_since_last_tick += delta
	if time_since_last_tick < FPS_30_DELTA:
		return
	time_since_last_tick -= FPS_30_DELTA

	_update_surface_objects()


func _update_surface_objects() -> void:
	for i in range(0, _surface_objects_ids.size()):
		var id: int = _surface_objects_ids[i]
		var position: Vector3 = _surface_objects_refs[i].global_transform.origin
		var rotation: Vector3 = _surface_objects_refs[i].rotation_degrees
		sm64_handler.surface_object_move(id, position, rotation)


func load_surface_object(mesh_instance: MeshInstance3D) -> void:
	var mesh_faces: PackedVector3Array = mesh_instance.get_mesh().get_faces()
	var position: Vector3 = mesh_instance.global_transform.origin
	var rotation: Vector3 = mesh_instance.rotation_degrees
	var surface_object_id: int = sm64_handler.surface_object_create(mesh_faces, position, rotation)
	_surface_objects_ids.push_back(surface_object_id)
	_surface_objects_refs.push_back(mesh_instance)
	# Clean up automaticaly if MeshInstance3D is removed from tree or freed
	mesh_instance.connect("tree_exiting",Callable(self,"delete_surface_object").bind(mesh_instance),CONNECT_ONE_SHOT)


func load_all_surface_objects() -> void:
	for mesh_instance in get_tree().get_nodes_in_group(surface_objects_group):
		load_surface_object(mesh_instance)


func delete_surface_object(mesh_instance: MeshInstance3D) -> void:
	var index := _surface_objects_refs.find(mesh_instance)
	if index == -1:
		return

	var id: int = _surface_objects_ids[index]
	sm64_handler.surface_object_delete(id)
	_surface_objects_refs.remove_at(index)
	_surface_objects_ids.remove_at(index)


func delete_all_surface_objects() -> void:
	for id in _surface_objects_ids:
		sm64_handler.surface_object_delete(id)

	_surface_objects_refs.clear()
	_surface_objects_ids.clear()
