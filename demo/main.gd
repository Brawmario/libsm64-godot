extends Node


@onready var pick_scenario: Control = $PickScenario
@onready var scenario_container: Node = $ScenarioContainer


func _ready() -> void:
	Events.return_to_menu_requested.connect(_on_return_to_menu_requested)


func _clear_scenario() -> void:
	for scenario in scenario_container.get_children():
		scenario.queue_free()


func _on_pick_scenario_scenario_picked(path) -> void:
	var packed_scene := load(path) as PackedScene
	if not packed_scene:
		push_error("Invalid scene path: ", path)

	_clear_scenario()

	var scene := packed_scene.instantiate()
	scenario_container.add_child(scene)

	pick_scenario.visible = false


func _on_return_to_menu_requested() -> void:
	_clear_scenario()

	pick_scenario.visible = true
