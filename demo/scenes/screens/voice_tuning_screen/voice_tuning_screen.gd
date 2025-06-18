extends Control
class_name VoiceTuningScreen

@onready
var speech_synth_node: SpeechSynthesizerNode = %SpeechSynthesizerNode
@onready
var root_list: ItemList = %RootList
@onready
var stress_list: ItemList = %StressList


var root_list_items: Array[String] = []
var stress_list_items: Array[String] = []

var current_root_phoneme: String = ""
var current_phoneme: Phoneme = null

func _ready() -> void:
	for p in speech_synth_node.lexicon.get_root_phoneme_catalog():
		root_list.add_item(p)
		root_list_items.append(p)
	stress_list.clear()
	
	root_list.item_selected.connect(_on_root_select)


func _on_root_select(index: int) -> void:
	_clear_main_area()
	stress_list.deselect_all()
	stress_list.clear()
	stress_list_items.clear()
	current_root_phoneme = root_list_items[index]
	current_phoneme = null
	for g in speech_synth_node.lexicon.get_phoneme_stress_variants_from_name(current_root_phoneme):
		stress_list_items.append(g)
		stress_list.add_item(g)
	if len(stress_list_items) <= 1:
		stress_list.select(0)

func _on_stress_select(index: int) -> void:
	_clear_main_area()
	current_phoneme = Phoneme.new()
	current_phoneme.full_name = stress_list_items[index]
	_load_main_area()

func _clear_main_area() -> void:
	pass

func _load_main_area() -> void:
	pass
