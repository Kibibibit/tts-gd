#include "speech_synthesizer_node.h"
#include <godot_cpp/core/class_db.hpp>
#include "lexicon.h"


using namespace godot;

SpeechSynthesizerNode::SpeechSynthesizerNode() {
}
SpeechSynthesizerNode::~SpeechSynthesizerNode() {
}

String SpeechSynthesizerNode::text_to_phoneme_string(const String &text) const {
	if (lexicon.is_null()) {
		UtilityFunctions::push_error("Lexicon is not set for SpeechSynthesizerNode.");
		return String();
	}
	return lexicon->translate(text);
}

TypedArray<Phoneme> SpeechSynthesizerNode::text_to_phoneme_array(const String &text) const {
	if (lexicon.is_null()) {
		UtilityFunctions::push_error("Lexicon is not set for SpeechSynthesizerNode.");
		return TypedArray<Phoneme>();
	}

	String phoneme_string = lexicon->translate(text);
	if (phoneme_string.is_empty()) {
		return TypedArray<Phoneme>();
	}

	TypedArray<Phoneme> phonemes;
	for (const String &phoneme_name : phoneme_string.split(" ")) {
		if (phoneme_name.is_empty()) {
			continue;
		}
		Ref<Phoneme> phoneme = memnew(Phoneme);
		phoneme->set_full_name(phoneme_name);
		phonemes.push_back(phoneme);
	}
	return phonemes;
}

Ref<Lexicon> SpeechSynthesizerNode::get_lexicon() const {
	return lexicon;
}
void SpeechSynthesizerNode::set_lexicon(const Ref<Lexicon> &lexicon) {
	if (this->lexicon == lexicon) {
		return;
	}
	this->lexicon = lexicon;
}


void SpeechSynthesizerNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("text_to_phoneme_string", "text"), &SpeechSynthesizerNode::text_to_phoneme_string);
	ClassDB::bind_method(D_METHOD("text_to_phoneme_array", "text"), &SpeechSynthesizerNode::text_to_phoneme_array);
	ClassDB::bind_method(D_METHOD("get_lexicon"), &SpeechSynthesizerNode::get_lexicon);
	ClassDB::bind_method(D_METHOD("set_lexicon", "lexicon"), &SpeechSynthesizerNode::set_lexicon);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lexicon", PROPERTY_HINT_RESOURCE_TYPE, "Lexicon"), "set_lexicon", "get_lexicon");
}