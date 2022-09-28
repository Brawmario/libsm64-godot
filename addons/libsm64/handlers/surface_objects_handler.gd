class_name SurfaceObjectsHandler
extends Node

const FPS_30_DELTA := 1.0/30.0
const RAD_TO_DEG_FACTOR := 180.0/PI

## SM64Handler instance
@export var sm64_handler: Resource
## Group name that contains the MeshInstance3D that are part of the scene's surface objects
@export var surface_objects_group := "libsm64_surface_objects"

var _surface_objects_ids: Array[int] = []
var _surface_objects_refs: Array[MeshInstance3D] = []
var time_since_last_tick := 0.0


func _physics_process(delta: float) -> void:
	time_since_last_tick += delta
	if time_since_last_tick < FPS_30_DELTA:
		return
	time_since_last_tick -= FPS_30_DELTA

	_update_surface_objects()


func _update_surface_objects() -> void:
	for i in range(0, _surface_objects_ids.size()):
		var id := _surface_objects_ids[i]
		var position := _surface_objects_refs[i].global_transform.origin
		var rotation_degrees := _surface_objects_refs[i].rotation * RAD_TO_DEG_FACTOR
		sm64_handler.surface_object_move(id, position, rotation_degrees)


## Load MeshInstance3D into the SM64Handler instance
func load_surface_object(mesh_instance: MeshInstance3D) -> void:
	var mesh_faces := mesh_instance.get_mesh().get_faces()
	var position := mesh_instance.global_transform.origin
	var rotation_degrees := mesh_instance.rotation * RAD_TO_DEG_FACTOR
	var surface_object_id: int = sm64_handler.surface_object_create(mesh_faces, position, rotation_degrees)
	_surface_objects_ids.push_back(surface_object_id)
	_surface_objects_refs.push_back(mesh_instance)
	# Clean up automaticaly if MeshInstance3D is removed from tree or freed
	mesh_instance.tree_exiting.connect(delete_surface_object.bind(mesh_instance), CONNECT_ONE_SHOT)


## Load all MeshInstance3D in surface_objects_group into the SM64Handler instance
func load_all_surface_objects() -> void:
	for node in get_tree().get_nodes_in_group(surface_objects_group):
		var mesh_instance := node as MeshInstance3D
		if not mesh_instance:
			push_warning("Non MeshInstance3D in %s group" % surface_objects_group)
			continue
		load_surface_object(mesh_instance)


## Delete MeshInstance3D from the SM64Handler instance if present
func delete_surface_object(mesh_instance: MeshInstance3D) -> void:
	var index := _surface_objects_refs.find(mesh_instance)
	if index == -1:
		return

	var id := _surface_objects_ids[index]
	sm64_handler.surface_object_delete(id)
	_surface_objects_refs.remove_at(index)
	_surface_objects_ids.remove_at(index)


## Delete all MeshInstance3D from the SM64Handler instance
func delete_all_surface_objects() -> void:
	for id in _surface_objects_ids:
		sm64_handler.surface_object_delete(id)

	_surface_objects_refs.clear()
	_surface_objects_ids.clear()
