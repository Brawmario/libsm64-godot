extends Spatial


const FPS_30_DELTA = 1.0/30.0

export var sm64_handler: Resource
export var camera_path: NodePath
export var stick_left := "mario_stick_left"
export var stick_right := "mario_stick_right"
export var stick_up := "mario_stick_up"
export var stick_down := "mario_stick_down"
export var input_a := "mario_a"
export var input_b := "mario_b"
export var input_z := "mario_z"

var camera: Camera
var mesh_instance: MeshInstance
var mesh: ArrayMesh
var material: SpatialMaterial
var id := -1
var time_since_last_tick := 0.0


func _ready() -> void:
	camera = get_node(camera_path)

	mesh_instance = MeshInstance.new()
	add_child(mesh_instance)
	mesh_instance.set_as_toplevel(true)
	mesh_instance.translation = Vector3.ZERO

	mesh = ArrayMesh.new()
	mesh_instance.mesh = mesh


func _physics_process(delta: float) -> void:
	time_since_last_tick += delta
	if time_since_last_tick < FPS_30_DELTA:
		return
	time_since_last_tick -= FPS_30_DELTA

	if id < 0:
		return

	var stick_x := Input.get_action_strength(stick_left) - Input.get_action_strength(stick_right)
	var stick_y := Input.get_action_strength(stick_up) - Input.get_action_strength(stick_down)
	var stick := Vector2(stick_x, stick_y)

	var look_direction := camera.get_global_transform().basis.z
	var cam_look := Vector2(look_direction.x, look_direction.z)

	var a := Input.is_action_pressed(input_a)
	var b := Input.is_action_pressed(input_b)
	var z := Input.is_action_pressed(input_z)

	var inputs := {
		"cam_look": cam_look,
		"stick": stick,
		"a": a,
		"b": b,
		"z": z,
	}

	var tick_output: Dictionary = sm64_handler.mario_tick(id, inputs)

	var position: Vector3 = tick_output["position"]
	translation = position

	var mesh_array: Array = tick_output["mesh_array"]
	mesh.clear_surfaces()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, mesh_array)
	mesh_instance.set_surface_material(0, material)


func global_init() -> void:
	if sm64_handler and not sm64_handler.is_init:
		sm64_handler.global_init()


func create() -> void:
	if sm64_handler and sm64_handler.is_init:
		id = sm64_handler.mario_create(to_global(translation))
		if id < 0:
			return

		var texture := ImageTexture.new()
		texture.create_from_image(sm64_handler.mario_image)

		var texture_material := SpatialMaterial.new()
		texture_material.albedo_texture = texture
		texture_material.flags_transparent = true

		material = SpatialMaterial.new()
		material.vertex_color_use_as_albedo = true
		material.next_pass = texture_material
