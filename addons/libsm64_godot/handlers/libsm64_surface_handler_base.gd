class_name LibSM64SurfaceHandlerBase
extends Node

## This node contains the common functionality between [LibSM64StaticSurfacesHandler] and [LibSM64SurfaceObjectsHandler].
## This node is not meant to be used directly. Use [LibSM64StaticSurfacesHandler] or [LibSM64SurfaceObjectsHandler] instead.


## Faces of a [BoxShape3D] with size [code]Vector3(1, 1, 1)[/code]. Used to derive the faces of other [BoxShape3D]s.
const UNIT_BOX_SHAPE_3D_FACES: PackedVector3Array = [Vector3(-0.5, 0.5, 0.5), Vector3(0.5, 0.5, 0.5), Vector3(-0.5, -0.5, 0.5), Vector3(0.5, 0.5, 0.5), Vector3(0.5, -0.5, 0.5), Vector3(-0.5, -0.5, 0.5), Vector3(0.5, 0.5, -0.5), Vector3(-0.5, 0.5, -0.5), Vector3(0.5, -0.5, -0.5), Vector3(-0.5, 0.5, -0.5), Vector3(-0.5, -0.5, -0.5), Vector3(0.5, -0.5, -0.5), Vector3(0.5, 0.5, 0.5), Vector3(0.5, 0.5, -0.5), Vector3(0.5, -0.5, 0.5), Vector3(0.5, 0.5, -0.5), Vector3(0.5, -0.5, -0.5), Vector3(0.5, -0.5, 0.5), Vector3(-0.5, 0.5, -0.5), Vector3(-0.5, 0.5, 0.5), Vector3(-0.5, -0.5, -0.5), Vector3(-0.5, 0.5, 0.5), Vector3(-0.5, -0.5, 0.5), Vector3(-0.5, -0.5, -0.5), Vector3(0.5, 0.5, 0.5), Vector3(-0.5, 0.5, 0.5), Vector3(0.5, 0.5, -0.5), Vector3(-0.5, 0.5, 0.5), Vector3(-0.5, 0.5, -0.5), Vector3(0.5, 0.5, -0.5), Vector3(-0.5, -0.5, 0.5), Vector3(0.5, -0.5, 0.5), Vector3(-0.5, -0.5, -0.5), Vector3(0.5, -0.5, 0.5), Vector3(0.5, -0.5, -0.5), Vector3(-0.5, -0.5, -0.5)]


## Find a [LibSM64SurfacePropertiesComponent] child in the given node and return its [LibSM64SurfaceProperties]
static func find_surface_properties(node: Node) -> LibSM64SurfaceProperties:
	for child in node.get_children():
		if child is LibSM64SurfacePropertiesComponent:
			return child.surface_properties
	return null


## Get the faces from supported nodes (support for [MeshInstance3D] and partial support for [CollisionObject3D] and [CollisionShape3D])
static func get_faces_from_node(node: Node3D) -> PackedVector3Array:
	if node is CollisionObject3D:
		return get_faces_from_collision_object_3d(node)
	elif node is CollisionShape3D:
		return get_faces_from_collision_shape_3d(node)
	elif node is MeshInstance3D:
		return node.get_mesh().get_faces()
	else:
		push_error("Unsupported node (%s) type: %s" % [node.name, node.get_class()])
		return PackedVector3Array()


## Get the faces from a [CollisionObject3D] (which can contain multiple [CollisionShape3D])
static func get_faces_from_collision_object_3d(collision_object: CollisionObject3D) -> PackedVector3Array:
	var faces := PackedVector3Array()
	for node in collision_object.get_children():
		var collision_shape := node as CollisionShape3D
		if not collision_shape:
			continue
		var collision_shape_faces := get_faces_from_collision_shape_3d(collision_shape)
		# Apply the local transform of the CollisionShape3D to the faces, since we want them in relation to their parent
		faces.append_array(transform_packed_vector3_array(collision_shape_faces, collision_shape.transform))
	return faces


## Get the faces from a [CollisionShape3D] (support for [BoxShape3D] and [ConcavePolygonShape3D] currently)
static func get_faces_from_collision_shape_3d(collision_shape: CollisionShape3D) -> PackedVector3Array:
	var shape = collision_shape.shape
	if shape is BoxShape3D:
		return get_faces_from_box_shape_3d(shape)
	elif shape is ConcavePolygonShape3D:
		return shape.get_faces()
	else:
		push_error("Unsupported shape type: ", shape.get_class())
		return PackedVector3Array()


## Get the faces from a [BoxShape3D]
static func get_faces_from_box_shape_3d(box: BoxShape3D) -> PackedVector3Array:
	var faces := PackedVector3Array()
	var faces_size := UNIT_BOX_SHAPE_3D_FACES.size()
	faces.resize(faces_size)
	for i in range(faces_size):
		# Copy the unit BoxShape3D faces vertices, scaling them to the size of the box
		faces.set(i, UNIT_BOX_SHAPE_3D_FACES[i] * box.size)
	return faces


## Apply a [Transform3D] to a all elements of a [PackedVector3Array]
static func transform_packed_vector3_array(array: PackedVector3Array, transform: Transform3D) -> PackedVector3Array:
	var ret := PackedVector3Array()
	var size := array.size()
	ret.resize(size)
	for i in range(size):
		ret.set(i, transform * array[i])
	return ret
