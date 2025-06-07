extends VBoxContainer
class_name FilterControls

@onready
var enabled_toggle: CheckBox = %EnabledToggle
@onready
var notch_toggle: CheckBox = %NotchToggle
@onready
var cutoff_slider: HSlider = %CutoffSlider
@onready
var cutoff_label: Label = %CutoffLabel
@onready
var quality_slider: HSlider = %QualitySlider
@onready
var quality_label: Label = %QualityLabel
@onready
var gain_slider: HSlider = %GainSlider
@onready
var gain_label: Label = %GainLabel

var _filter: FormantFilter

var filter_set: bool = false
var _sample_rate: float = 0.0

func _ready() -> void:
	enabled_toggle.toggled.connect(_on_change)
	notch_toggle.toggled.connect(_on_change)
	cutoff_slider.value_changed.connect(_on_change)
	quality_slider.value_changed.connect(_on_change)
	gain_slider.value_changed.connect(_on_change)

func set_filter(f: FormantFilter, sample_rate: float):
	_filter = f
	_filter.stages = FormantFilter.STAGE_4
	_sample_rate = sample_rate
	filter_set = true
	_update_filter()
	
func _update_filter() -> void:
	if filter_set:
		_filter.set_amplification_gain(gain_slider.value)
		_filter.set_cutoff(cutoff_slider.value)
		_filter.set_resonance(quality_slider.value)
		if (notch_toggle.button_pressed):
			_filter.set_type(FormantFilter.NOTCH)
		else:
			_filter.set_type(FormantFilter.BANDPASS)
	

func _on_change(_args) -> void:
	_update_filter()
	cutoff_label.text = "Cutoff: %04d Hz" % cutoff_slider.value
	quality_label.text = "Quality: %.2f" % quality_slider.value
	gain_label.text = "Gain: %d dB" % gain_slider.value

func reset_filter() -> void:
	_on_change([]);
