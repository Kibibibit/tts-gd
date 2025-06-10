extends Node


@export
var voice: VoiceResource
@onready
var player: AudioStreamPlayer = $AudioStreamPlayer
@onready
var audio_controls: AudioControlButtons = %AudioControlButtons

@onready
var slider_batch: SliderBatch = %SliderBatch

@onready
var frame_slider: HSlider = %FrameSlider
@onready
var play_frame_button: CheckButton = %PlayFrameButton
@onready
var frame_label: Label = %FrameNumberLabel
@onready
var frames_container: GridContainer = %FramesContainer
@onready
var add_frame_button: Button = %AddButton
@onready
var play_sequence_button: Button = %PlaySequenceButton
var playback: AudioStreamGeneratorPlayback

var playing_sequence: bool = false

var synth: LPCSynthesizer = LPCSynthesizer.new()
var wave_generator := WaveGenerator.new()

var sequence: Array[int] = []
var buttons: Array[Button] = []

var play_file: bool = true

var frame_index: int = 0

var time_per_frame: float = 0.2
var frame_progress: float = 0.0
var sequence_index: int = 0

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
	play_frame_button.toggled.connect(_change_play_mode)
	synth_params = slider_batch.get_values()
	frame_slider.editable = false
	frame_slider.min_value = 0
	frame_slider.max_value = voice.lpc_file.get_frame_count()
	frame_slider.value = 0
	frame_slider.value_changed.connect(_set_frame_index)
	
	slider_batch.value_changed.connect(_on_slider_value_changed)
	
	add_frame_button.pressed.connect(_on_frame_add)
	
	play_sequence_button.pressed.connect(_play_sequence)

func _process(_delta: float) -> void:
	
	

	var frames: int = playback.get_frames_available()
		
	var b := PackedVector2Array()
	b.resize(frames)
	if play_file:
		var f := synth.next_n_playback_samples(frames)
		frame_index = synth.get_current_frame_index()
		for i in frames:
			b[i] = f[i]*Vector2.ONE
		frame_slider.value = frame_index
	else:
		if not playing_sequence:
			for i in frames:
				b[i] = synth.get_next_sample_at(frame_index)*Vector2.ONE
		else:
			for i in frames:
				var s := sequence[sequence_index]
				var s_n_i := posmod(sequence_index+1, len(sequence))
				var n_s := sequence[s_n_i]
				b[i] = synth.get_next_sample_at_interpolated(s, n_s, frame_progress/time_per_frame)*Vector2.ONE
				frame_progress += voice.lpc_file.get_sampling_period()
				if frame_progress > time_per_frame:
					frame_progress = 0
					sequence_index = s_n_i
	
		
	
	frame_label.text = "Frame No. %d" % frame_index
	
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

func _change_play_mode(v: bool) -> void:
	play_file = not v
	frame_slider.editable = v
	synth.reset()
	playback.stop()
	playback.clear_buffer()
	playback.start()

func _set_frame_index(v: float) -> void:
	if not play_file:
		frame_index = floori(v)

func _on_frame_add() -> void:
	sequence.append(frame_index)
	var b := Button.new()
	b.text = "%d" % frame_index
	b.pressed.connect(_on_frame_remove.bind(buttons.size()))
	frames_container.add_child(b)
	buttons.append(b)
	
func _on_frame_remove(index: int) -> void:
	var b := buttons[index]
	b.pressed.disconnect(_on_frame_remove)
	b.queue_redraw()
	frames_container.remove_child(b)
	buttons.remove_at(index)
	sequence.remove_at(index)
	if playing_sequence:
		playing_sequence = false
	if not buttons.is_empty():
		for i in range(len(buttons)):
			var b2 := buttons[i]
			b2.pressed.disconnect(_on_frame_remove)
			b2.pressed.connect(_on_frame_remove.bind(i))

func _play_sequence() -> void:
	if playing_sequence:
		playing_sequence = false
		return
	player.stop()
	playback.stop()
	playback.clear_buffer()
	play_file = false
	play_frame_button.button_pressed = true
	frame_slider.editable = true
	frame_progress = 0.0
	playing_sequence = true
	sequence_index = 0
	player.play()
	playback = player.get_stream_playback()
	playback.start()
	
