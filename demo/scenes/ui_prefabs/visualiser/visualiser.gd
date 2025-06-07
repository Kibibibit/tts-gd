extends Control
class_name Visualiser

#region constants

const _BUCKET_COUNT_PARAM: StringName = &"bucket_count"
const _BUCKET_DATA_PARAM: StringName = &"bucket_data"
const _BG_COLOR_PARAM: StringName = &"bg_color"
const _FG_COLOR_PARAM: StringName = &"fg_color"
const _HEIGHT_SCALE_PARAM: StringName = &"height_scale"
#endregion

#region sliders and labels
@onready
var bucket_count_slider: HSlider = %BucketCountSlider
@onready
var bucket_count_label: Label = %BucketCountLabel
@onready
var height_scale_slider: HSlider = %HeightScaleSlider
@onready
var height_scale_label: Label = %HeightScaleLabel
@onready
var min_db_slider: HSlider = %MinDBSlider
@onready
var min_db_label: Label = %MinDBLabel
@onready
var max_frequency_slider: HSlider = %MaxFSlider
@onready
var max_frequency_label: Label = %MaxFLabel
#endregion

@onready
var rect: ColorRect = %VisualiserRect
@onready
var shader: ShaderMaterial = rect.material



@export
var bg_color: Color = Color.TRANSPARENT:
	get:
		return bg_color
	set(v):
		bg_color = v
		if is_inside_tree():
			shader.set_shader_parameter(_BG_COLOR_PARAM, v)

@export
var fg_color: Color = Color.WHITE:
	get:
		return fg_color
	set(v):
		fg_color = v
		if is_inside_tree():
			shader.set_shader_parameter(_FG_COLOR_PARAM, v)


@export_range(0,1000,1)
var bucket_count: int = 500:
	get:
		return bucket_count
	set(v):
		bucket_count = v
		max_bucket_data.resize(v)
		min_bucket_data.resize(v)
		current_bucket_data.resize(v)
		if is_inside_tree():
			shader.set_shader_parameter(_BUCKET_COUNT_PARAM, v)
			bucket_count_label.text = "Bucket Count: %d" % v

@export_range(0.0, 30.0, 0.01)
var height_scale: float = 1.0:
	get:
		return height_scale
	set(v):
		height_scale = v
		if is_inside_tree():
			shader.set_shader_parameter(_HEIGHT_SCALE_PARAM, v)
			height_scale_label.text = "Height Scale: %.2f" % v

@export_range(0, 100, 0.5)
var min_db: float = 60.0:
	get:
		return min_db
	set(v):
		min_db = v
		if is_inside_tree():
			min_db_label.text = "Min Db: %.1f" % v

@export_range(0,10000, 1)
var max_frequency: float = 5000.0:
	get:
		return max_frequency
	set(v):
		max_frequency = v
		if is_inside_tree():
			max_frequency_label.text = "Max Frequency: %d Hz" % v

@export_range(0,1,0.01)
var animation_speed: float = 0.1

var max_bucket_data: PackedFloat32Array = []
var current_bucket_data: PackedFloat32Array = []
var min_bucket_data: PackedFloat32Array = []

var spectrum: AudioEffectSpectrumAnalyzerInstance

func _ready():
	spectrum = AudioServer.get_bus_effect_instance(0,0)
	max_bucket_data = PackedFloat32Array()
	max_bucket_data.resize(bucket_count)
	min_bucket_data = PackedFloat32Array()
	min_bucket_data.resize(bucket_count)
	current_bucket_data.resize(bucket_count)
	shader.set_shader_parameter(_BG_COLOR_PARAM, bg_color)
	shader.set_shader_parameter(_FG_COLOR_PARAM, fg_color)
	## Force shader param and label setting
	bucket_count = bucket_count
	height_scale = height_scale
	min_db = min_db
	max_frequency = max_frequency
	
	bucket_count_slider.value_changed.connect(_update_bucket_count)
	bucket_count_slider.value = bucket_count
	
	height_scale_slider.value_changed.connect(_update_height_scale)
	height_scale_slider.value = height_scale
	
	min_db_slider.value_changed.connect(_update_min_db)
	min_db_slider.value = min_db
	
	max_frequency_slider.value_changed.connect(_update_max_frequency)
	max_frequency_slider.value = max_frequency

func _process(_delta: float) -> void:
	
	var bucket_width: float = max_frequency / bucket_count
	var prev_hz: float = 0.0
	for i in range(bucket_count):
		
		var hz = (i+1) * bucket_width
		var magnitude = spectrum.get_magnitude_for_frequency_range(prev_hz, hz).length()
		var energy = clampf((min_db + linear_to_db(magnitude)) / min_db, 0.0, 1.0)
		var height = energy 
		
		if height > max_bucket_data[i]:
			max_bucket_data[i] = height
		else:
			max_bucket_data[i] = lerp(max_bucket_data[i], height, animation_speed)
		
		if height <= 0.0:
			min_bucket_data[i] = lerp(min_bucket_data[i], 0.0, animation_speed)
		
		prev_hz = hz
		
		
		
		var min_height = min_bucket_data[i]
		var max_height = max_bucket_data[i]
		current_bucket_data[i] = lerp(min_height, max_height, animation_speed)
	
	shader.set_shader_parameter(_BUCKET_DATA_PARAM, current_bucket_data)
		
		
	
	

#region slider update callbacks

func _update_bucket_count(v: float) -> void:
	bucket_count = roundi(v)

func _update_height_scale(v: float) -> void:
	height_scale = v

func _update_min_db(v: float) -> void:
	min_db = v

func _update_max_frequency(v: float) -> void:
	max_frequency = v

#endregion
