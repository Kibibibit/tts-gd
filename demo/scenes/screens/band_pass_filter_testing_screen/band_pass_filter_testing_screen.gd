extends Control

const _ID_PINK_NOISE: int = 0
const _ID_WHITE_NOISE: int = 1
const _ID_SAW_WAVE: int = 2

@onready
var start_button: Button = %StartButton
@onready
var stop_button: Button = %StopButton
@onready
var player: AudioStreamPlayer = $AudioStreamPlayer
@onready
var noise_selector: OptionButton = %NoiseDropdown
@onready
var filter_controls_a: FilterControls = %FilterControlsA
@onready
var filter_controls_b: FilterControls = %FilterControlsB

var noise_type: int = _ID_PINK_NOISE

var playback: AudioStreamGeneratorPlayback
var wave_generator: WaveGenerator
var filterA: FormantFilter
var filterB: FormantFilter

var filters: Array[FormantFilter] = []
var filter_controls: Array[FilterControls] = []

var phase: float = 0.0
var phase_step: float = 0.0

func _ready() -> void:
	player.play()
	player.stream_paused = true
	playback = player.get_stream_playback()
	wave_generator = WaveGenerator.new()
	
	filterA = FormantFilter.new()
	filterB = FormantFilter.new()
	filters.append(filterA)
	filters.append(filterB)
	filter_controls_a.set_filter(filterA, player.stream.mix_rate)
	filter_controls.append(filter_controls_a)
	filter_controls_b.set_filter(filterB, player.stream.mix_rate)
	filter_controls.append(filter_controls_b)
	start_button.pressed.connect(_start)
	stop_button.pressed.connect(_stop)
	
	noise_selector.item_selected.connect(_change_noise)
	
	phase_step = 100.0 / player.stream.mix_rate


func _process(_delta: float) -> void:
	
	var frames: int = playback.get_frames_available()
	
	
	var buffer: PackedVector2Array = PackedVector2Array()
	var mono_buffer: PackedFloat32Array = PackedFloat32Array()
	buffer.resize(frames)
	mono_buffer.resize(frames)
	
	for i in frames:
		phase = fmod(phase+phase_step, 1.0)
		mono_buffer[i] = _get_signal()
		
	var any_filters: bool = false
	for i in range(len(filters)):
		if filter_controls[i].enabled_toggle.button_pressed:
			any_filters = true
			var b: PackedFloat32Array = mono_buffer.duplicate()
			filters[i].process_buffer(b)
			for j in frames:
				buffer[j] += b[j] * Vector2.ONE
	
	if not any_filters:
		for j in frames:
				buffer[j] += mono_buffer[j] * Vector2.ONE
	playback.push_buffer(buffer)
	

func _get_signal() -> float:
	match (noise_type):
		_ID_PINK_NOISE:
			return wave_generator.next_pink_noise()
		_ID_WHITE_NOISE:
			return wave_generator.next_white_noise() * 0.45
		_ID_SAW_WAVE:
			return wave_generator.voiced_wave(phase, 0.1)
		_:
			return 0.0


func _start() -> void:
	player.stream_paused = false
	start_button.disabled = true
	stop_button.disabled = false
	for c in filter_controls:
		c.reset_filter()
func _stop() -> void:
	player.stream_paused = true
	start_button.disabled = false
	stop_button.disabled = true

func _change_noise(v: int) -> void:
	noise_type = v
	
