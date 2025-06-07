extends Control

@onready
var visualiser: Visualiser = %Visualiser
@onready
var player: AudioStreamPlayer = %AudioStreamPlayer
@onready
var start_button: Button = %StartButton
@onready
var stop_button: Button = %StopButton


func _ready() -> void:
	player.stream = AudioStreamMP3.load_from_file("res://assets/tenebre_rosso_sangue.mp3")
	
	start_button.disabled = false
	stop_button.disabled = true
	
	start_button.pressed.connect(_start)
	stop_button.pressed.connect(_stop)
	
	
func _start() -> void:
	player.play(0)
	start_button.disabled = true
	stop_button.disabled = false

func _stop() -> void:
	player.stop()
	start_button.disabled = false
	stop_button.disabled = true
