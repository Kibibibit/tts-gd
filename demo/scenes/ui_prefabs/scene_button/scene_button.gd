extends Button
class_name SceneButton


@export
var scene: PackedScene



func _ready() -> void:
	self.pressed.connect(_change_scene)
	
	
func _change_scene() -> void:
	if scene:
		SignalBus.change_scene.emit(scene)
