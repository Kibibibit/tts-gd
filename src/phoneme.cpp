#include "phoneme.h"

using namespace godot;


Phoneme::Phoneme() {
	voice_mix_ratio = 0.5f; // Default mix ratio
	formant_set = Ref<FormantSet>(memnew(FormantSet()));
	end_formant_set = Ref<FormantSet>(memnew(FormantSet()));
	envelope = Ref<Envelope>(memnew(Envelope()));
	symbol = "";
	ipa_symbol = "";
	description = "";
	f0_modifier = 1.0; // Default F0 modifier
	is_diphthong = false; // Default to not being a diphthong
	is_silence = false; // Default to not being silence
	transition_start = 0.0; // Default transition start
	transition_length = 0.0; // Default transition length
}
Phoneme::~Phoneme() {
	// Destructor logic if needed
}
double Phoneme::get_typical_duration() const {
	return envelope->get_total_time();
}
void Phoneme::set_symbol(const String &symbol) {
	this->symbol = symbol;
}
String Phoneme::get_symbol() const {
	return symbol;
}
void Phoneme::set_ipa_symbol(const String &ipa_symbol) {
	this->ipa_symbol = ipa_symbol;
}
String Phoneme::get_ipa_symbol() const {
	return ipa_symbol;
}
void Phoneme::set_description(const String &description) {
	this->description = description;
}
String Phoneme::get_description() const {
	return description;
}
void Phoneme::set_voice_mix_ratio(double ratio) {
	voice_mix_ratio = ratio;
}
double Phoneme::get_voice_mix_ratio() const {
	return voice_mix_ratio;
}
void Phoneme::set_formant_set(const Ref<FormantSet> &formant_set) {
	this->formant_set = formant_set;
}
Ref<FormantSet> Phoneme::get_formant_set() const {
	return formant_set;
}

Ref<FormantSet> Phoneme::get_end_formant_set() const {
	return end_formant_set;
}
void Phoneme::set_end_formant_set(const Ref<FormantSet> &end_formant_set) {
	this->end_formant_set = end_formant_set;
}

void Phoneme::set_envelope(const Ref<Envelope> &envelope) {
	this->envelope = envelope;
}
Ref<Envelope> Phoneme::get_envelope() const {
	return envelope;
}
void Phoneme::set_f0_modifier(double modifier) {
	f0_modifier = modifier;
}
double Phoneme::get_f0_modifier() const {
	return f0_modifier;
}

void Phoneme::set_is_diphthong(bool is_diphthong) {
	this->is_diphthong = is_diphthong;
}
bool Phoneme::get_is_diphthong() const {
	return is_diphthong;
}
void Phoneme::set_is_silence(bool is_silence) {
	this->is_silence = is_silence;
}
bool Phoneme::get_is_silence() const {
	return is_silence;
}
void Phoneme::set_transition_start(double start) {
	transition_start = start;
}
double Phoneme::get_transition_start() const {
	return transition_start;
}
void Phoneme::set_transition_length(double length) {
	transition_length = length;
}
double Phoneme::get_transition_length() const {
	return transition_length;
}
Ref<FormantSet> Phoneme::get_effective_formant_set(double t) const {

	if (is_silence) {
		return Ref<FormantSet>(memnew(FormantSet())); // Return an empty formant set for silence
	}
	if (!is_diphthong) {
		return formant_set; // If not a diphthong, return the regular formant set
	}

	double duration = get_typical_duration();

	double t_mapped = UtilityFunctions::clampf(t / duration, 0.0, 1.0);
	if (t_mapped < transition_start) {
		return formant_set;
	} else if (t_mapped > (transition_start + transition_length)) {
		return end_formant_set;
	} else {
		double transition_t = (t_mapped - transition_start) / transition_length;
		return formant_set->blend(end_formant_set, transition_t);
	}

}




void Phoneme::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_typical_duration"), &Phoneme::get_typical_duration);
	ClassDB::bind_method(D_METHOD("set_symbol", "symbol"), &Phoneme::set_symbol);
	ClassDB::bind_method(D_METHOD("get_symbol"), &Phoneme::get_symbol);
	ClassDB::bind_method(D_METHOD("set_ipa_symbol", "ipa_symbol"), &Phoneme::set_ipa_symbol);
	ClassDB::bind_method(D_METHOD("get_ipa_symbol"), &Phoneme::get_ipa_symbol);
	ClassDB::bind_method(D_METHOD("set_description", "description"), &Phoneme::set_description);
	ClassDB::bind_method(D_METHOD("get_description"), &Phoneme::get_description);
	ClassDB::bind_method(D_METHOD("set_voice_mix_ratio", "ratio"), &Phoneme::set_voice_mix_ratio);
	ClassDB::bind_method(D_METHOD("get_voice_mix_ratio"), &Phoneme::get_voice_mix_ratio);
	ClassDB::bind_method(D_METHOD("set_formant_set", "formant_set"), &Phoneme::set_formant_set);
	ClassDB::bind_method(D_METHOD("get_formant_set"), &Phoneme::get_formant_set);
	ClassDB::bind_method(D_METHOD("get_end_formant_set"), &Phoneme::get_end_formant_set);
	ClassDB::bind_method(D_METHOD("set_end_formant_set", "end_formant_set"), &Phoneme::set_end_formant_set);
	ClassDB::bind_method(D_METHOD("set_envelope", "envelope"), &Phoneme::set_envelope);
	ClassDB::bind_method(D_METHOD("get_envelope"), &Phoneme::get_envelope);
	ClassDB::bind_method(D_METHOD("set_f0_modifier", "modifier"), &Phoneme::set_f0_modifier);
	ClassDB::bind_method(D_METHOD("get_f0_modifier"), &Phoneme::get_f0_modifier);
	ClassDB::bind_method(D_METHOD("set_is_diphthong", "is_diphthong"), &Phoneme::set_is_diphthong);
	ClassDB::bind_method(D_METHOD("get_is_diphthong"), &Phoneme::get_is_diphthong);
	ClassDB::bind_method(D_METHOD("set_is_silence", "is_silence"), &Phoneme::set_is_silence);
	ClassDB::bind_method(D_METHOD("get_is_silence"), &Phoneme::get_is_silence);
	ClassDB::bind_method(D_METHOD("set_transition_start", "start"), &Phoneme::set_transition_start);
	ClassDB::bind_method(D_METHOD("get_transition_start"), &Phoneme::get_transition_start);
	ClassDB::bind_method(D_METHOD("set_transition_length", "length"), &Phoneme::set_transition_length);
	ClassDB::bind_method(D_METHOD("get_transition_length"), &Phoneme::get_transition_length);
	ClassDB::bind_method(D_METHOD("get_effective_formant_set", "t"), &Phoneme::get_effective_formant_set);
	

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "symbol"), "set_symbol", "get_symbol");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "ipa_symbol"), "set_ipa_symbol", "get_ipa_symbol");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "voice_mix_ratio", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_voice_mix_ratio", "get_voice_mix_ratio");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "f0_modifier", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_f0_modifier", "get_f0_modifier");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "transition_start", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_transition_start", "get_transition_start");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "transition_length", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_transition_length", "get_transition_length");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_diphthong"), "set_is_diphthong", "get_is_diphthong");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_silence"), "set_is_silence", "get_is_silence");
}