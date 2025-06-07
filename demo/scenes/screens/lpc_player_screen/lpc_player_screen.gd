extends Node


@export
var lpc: LPCFile

@onready
var player: AudioStreamPlayer = $AudioStreamPlayer

var playback: AudioStreamGeneratorPlayback

var synth: LPCSynthesizer = LPCSynthesizer.new()
var wave_generator := WaveGenerator.new()



func _ready() -> void:
	player.play()
	playback = player.get_stream_playback()
	synth.lpc_file = lpc
	synth.wave_generator = wave_generator
	player.stream.mix_rate = lpc.get_sample_rate()
	synth.base_frequency = 120.0
	synth.breath = 1.0
	synth.max_order = lpc.get_max_n_coefficients()

func _process(_delta: float) -> void:
	
	var frames: int = playback.get_frames_available()
	
	var frame_position = synth.progress / lpc.get_dx()
	var frame_index = int(floor(frame_position)) % lpc.get_frame_count()
	
	print("Frame: %d/%d (%.2f%%)" % [
		frame_index, 
		lpc.get_frame_count(),
		(frame_index / float(lpc.get_frame_count())) * 100.0
	])
	
	var b := PackedVector2Array()
	var f := synth.next_n_samples(frames)
	b.resize(frames)
	for i in frames:
		b[i] = f[i]*Vector2.ONE
		
	print(synth.progress)
	playback.push_buffer(b)
