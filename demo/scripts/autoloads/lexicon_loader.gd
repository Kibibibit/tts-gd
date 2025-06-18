extends Node


var _lexicon: Lexicon
var _lexicon_path: String = "res://resources/cmudict_full.dict"
var loaded: bool = false
	

func get_lexicon() -> Lexicon:
	if not loaded:
		_lexicon = load(_lexicon_path)
		loaded=true
	return _lexicon
