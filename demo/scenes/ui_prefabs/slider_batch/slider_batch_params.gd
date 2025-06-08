extends Resource
class_name SliderBatchParams

enum PropertyType {
	FLOAT,
	INT
}

@export
var property_name: StringName
@export
var label_string_prefix: String = ""
@export
var minimum: float = 0.0
@export
var maximum: float = 100.0
@export
var step: float = 1.0
@export
var default_value: float = 0.0
@export
var is_exponential: bool = false
@export
var display_digits: int = 1
@export
var property_type: PropertyType = PropertyType.FLOAT
