extends Node


@export
var lpc: LPCFile
@onready
var player: AudioStreamPlayer = $AudioStreamPlayer
@onready
var audio_controls: AudioControlButtons = %AudioControlButtons


var playback: AudioStreamGeneratorPlayback

var synth: LPCSynthesizer = LPCSynthesizer.new()
var wave_generator := WaveGenerator.new()

var breath: float = 0.0
var base_frequency: = 150.0
var voice_shape: float = 0.5


func _ready() -> void:
	player.play()
	player.stream_paused = not audio_controls.playing
	playback = player.get_stream_playback()
	synth.lpc_file = lpc
	synth.wave_generator = wave_generator
	player.stream.mix_rate = lpc.get_sample_rate()
	synth.base_frequency = 150.0
	synth.breath = 1.0
	synth.max_order = lpc.get_max_n_coefficients()
	audio_controls.toggle_play.connect(_pause_play)

func _process(_delta: float) -> void:
	
	var frames: int = playback.get_frames_available()
	
	var b := PackedVector2Array()
	var f := synth.next_n_samples(frames)
	b.resize(frames)
	for i in frames:
		b[i] = f[i]*Vector2.ONE
		
	playback.push_buffer(b)

func _pause_play(play: bool) -> void:
	player.stream_paused = not play
