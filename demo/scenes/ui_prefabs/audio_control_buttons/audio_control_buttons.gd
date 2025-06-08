extends Control
class_name AudioControlButtons

signal toggle_play(play: bool)

@onready
var play_button: Button = %PlayButton
@onready
var pause_button: Button = %PauseButton

var playing: bool = false:
	get:
		return playing
	set(v):
		if is_inside_tree():
			play_button.disabled = v
			pause_button.disabled = not v
			playing = v

func _ready() -> void:
	play_button.pressed.connect(_on_press.bind(true))
	pause_button.pressed.connect(_on_press.bind(false))
	play_button.disabled = playing
	pause_button.disabled = not playing



func _on_press(v: bool):
	playing = v
	toggle_play.emit(v)
