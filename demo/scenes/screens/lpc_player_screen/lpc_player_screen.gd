extends Node


@export
var voice: VoiceResource
@onready
var player: AudioStreamPlayer = $AudioStreamPlayer
@onready
var audio_controls: AudioControlButtons = %AudioControlButtons

@onready
var slider_batch: SliderBatch = %SliderBatch


var playback: AudioStreamGeneratorPlayback

var synth: LPCSynthesizer = LPCSynthesizer.new()
var wave_generator := WaveGenerator.new()



var synth_params: Dictionary[StringName, float] = {
	&"base_frequency": 0.0,
	&"wave_shape": 0.0,
	&"breath": 0.0,
	&"shape_gain_factor": 0.0,
	&"consonant_upper_threshold": 0.0,
	&"consonant_lower_threshold": 0.0,
	&"consonant_pre_emphasis_factor": 0.0,
	&"voiced_gain_factor": 0.0,
	&"voiced_coeff_stability_factor": 0.0,


}

func _ready() -> void:
	player.play()
	player.stream_paused = not audio_controls.playing
	playback = player.get_stream_playback()
	synth.voice = voice
	synth.wave_generator = wave_generator
	player.stream.mix_rate = voice.lpc_file.get_sample_rate()
	synth.max_order = voice.lpc_file.get_max_n_coefficients()
	audio_controls.toggle_play.connect(_pause_play)

	synth_params = slider_batch.get_values()

	slider_batch.value_changed.connect(_on_slider_value_changed)


func _process(_delta: float) -> void:
	
	var frames: int = playback.get_frames_available()
	
	var b := PackedVector2Array()
	var f := synth.next_n_playback_samples(frames)
	b.resize(frames)
	for i in frames:
		b[i] = f[i]*Vector2.ONE
		
	playback.push_buffer(b)

func _pause_play(play: bool) -> void:
	player.stream_paused = not play

func _on_slider_value_changed(p_name: StringName, value: float) -> void:
	synth_params[p_name] = value

	synth.base_frequency = synth_params[&"base_frequency"]
	synth.wave_shape = synth_params[&"wave_shape"]
	synth.breath = synth_params[&"breath"]
	synth.consonant_upper_threshold = synth_params[&"consonant_upper_threshold"]
	synth.consonant_lower_threshold = synth_params[&"consonant_lower_threshold"]
	synth.consonant_pre_emphasis_factor = synth_params[&"consonant_pre_emphasis_factor"]
	synth.voiced_gain_factor = synth_params[&"voiced_gain_factor"]
	synth.voiced_coeff_stability_factor = synth_params[&"voiced_coeff_stability_factor"]
	synth.shape_gain_factor = synth_params[&"shape_gain_factor"]
