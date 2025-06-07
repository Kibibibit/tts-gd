extends Node


@export
var lpc: LPCFile

@onready
var player: AudioStreamPlayer = $AudioStreamPlayer

var playback: AudioStreamGeneratorPlayback
var wave: WaveGenerator = WaveGenerator.new()

var synth: LPCSynthesizer = LPCSynthesizer.new()

var current_time = 0.0
var frame_duration = 0.0
@onready
var sample_duration = 1.0 / player.stream.mix_rate
var phase: int = 0

func _ready() -> void:
	player.play()
	playback = player.get_stream_playback()
	synth.lpc_file = lpc
	frame_duration = lpc.get_dx()

func _process(_delta: float) -> void:
	
	var frames: int = playback.get_frames_available()
	
	var b := PackedVector2Array()
	b.resize(frames)
	for i in frames:
		var frame_position = current_time / frame_duration
		var frame_index = int(frame_position) % lpc.get_frame_count()
		var next_frame_index = (frame_index + 1) % lpc.get_frame_count()
		var frame_alpha = fmod(frame_position, 1.0)
		var e := wave.bisqwit(phase, player.stream.mix_rate, 200.0, 0.1, 0.9)
		phase = wrapi(phase+1, 0, lpc.get_frame_count())

		var sample = synth.process_sample_interpolated(e, frame_index, next_frame_index, frame_alpha)
		
		b[i] = Vector2.ONE * sample
		current_time += sample_duration
		
	playback.push_buffer(b)
		
