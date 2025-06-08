extends VBoxContainer
class_name SliderBatch


signal value_changed(param: StringName, value: float)

@onready
var slider_grid: GridContainer = %SliderGrid
@onready
var defaults_button: Button = %DefaultsButton
@export
var parameters: Array[SliderBatchParams] = []


var _param_indices: Dictionary[StringName, int] = {}
var _sliders: Dictionary[StringName, HSlider] = {}
var _labels: Dictionary[StringName, Label] = {}
var _max_label_length: float = 0.0

var _values: Dictionary[StringName, float] = {}


func _ready() -> void:
	
	defaults_button.pressed.connect(_reset_to_default)
	
	for child in slider_grid.get_children():
		child.queue_free()
		slider_grid.remove_child(child)
		
	var i: int = 0
	for p in parameters:
		_param_indices[p.property_name] = i
		i+=1
		_values[p.property_name] = p.default_value
		
		var slider := HSlider.new()
		slider.min_value = max(1.0, p.minimum) if p.is_exponential else p.minimum
		slider.max_value = p.maximum
		slider.value = p.default_value
		slider.exp_edit = p.is_exponential
		slider.step = p.step
		
		slider.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		slider.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		
		slider.value_changed.connect(_on_update.bind(p.property_name))
		
		
		_sliders[p.property_name] = slider
		
		slider_grid.add_child(slider)
		
		var label := Label.new()
		
		label.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		
		_labels[p.property_name] = label
		slider_grid.add_child(label)
		
		_update_label_text(p.property_name)
		
		var max_label_size := _max_label_text_length(p.property_name)
		_max_label_length = max(_max_label_length, max_label_size)
	
	for p in _labels.keys():
		var label : Label = _labels[p]
		label.custom_minimum_size.x = _max_label_length
	_reset_to_default()

func _get_label_text(param: StringName, value: float) -> String:
	var p := parameters[_param_indices[param]]
	var format_string: String = p.label_string_prefix
	if format_string.length() > 0:
		format_string += " "
	if p.property_type == SliderBatchParams.PropertyType.FLOAT:
		format_string += "%." + ("%d" % p.display_digits) + "f"
		return format_string % value
	else:
		format_string += "%0" + ("%d" % p.display_digits) + "d"
		return format_string % roundi(value)
	
	

func _update_label_text(param: StringName) -> void:
	_labels[param].text = _get_label_text(param, _values[param])

func _max_label_text_length(param: StringName) -> float:
	var p := parameters[_param_indices[param]]
	var string0 := _get_label_text(param, p.minimum)
	var string1 := _get_label_text(param, p.maximum)
	
	var font := _labels[param].get_theme_default_font()
	var x0 = font.get_string_size(string0).x
	var x1 = font.get_string_size(string1).x
	return max(x0, x1)


func _on_update(value: float, param: StringName) -> void:
	_values[param] = value
	_update_label_text(param)
	value_changed.emit(param, value)

func get_values() -> Dictionary[StringName, float]:
	return _values.duplicate()

func set_value(param: StringName, value: float) -> void:
	if _values.has(param):
		_sliders[param].value = value
		_on_update(value, param)
	else:
		push_warning("Tried to set unkown param in SliderBatch")

func get_value(param: StringName) -> float:
	if _values.has(param):
		return _values[param]
	else:
		push_warning("Tried to access unknown param in SliderBatch")
		return 0.0

func _reset_to_default() -> void:
	for parameter in parameters:
		_sliders[parameter.property_name].value = parameter.default_value
		_on_update(parameter.default_value, parameter.property_name)
		
