extends Node
class_name RootScene

@export
var initial_scene: PackedScene

var current_scene: Node

func _ready() -> void:
	
	_set_scene(initial_scene)
	SignalBus.change_scene.connect(_set_scene)
	SignalBus.return_to_main.connect(_set_scene.bind(initial_scene))

func _exit_tree() -> void:
	SignalBus.change_scene.disconnect(_set_scene)

func _set_scene(new_scene: PackedScene):
	if current_scene:
		remove_child(current_scene)
	current_scene = new_scene.instantiate()
	add_child(current_scene)
