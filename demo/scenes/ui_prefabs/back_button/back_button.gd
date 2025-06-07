extends Button
class_name BackButton


func _ready() -> void:
	self.pressed.connect(_go_back)

func _go_back() -> void:
	SignalBus.return_to_main.emit()
