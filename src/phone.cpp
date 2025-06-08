#include "phone.h"

using namespace godot;

Phone::Phone()
{
	name = "";
	phoneme_ipa = "";
	phoneme_variant = "";
	start_lpc_indices = TypedArray<int>();
	end_lpc_indices = TypedArray<int>();
	transition_curve = TypedArray<double>();
	attack_time = 0.0;
	release_time = 0.0;
	is_diphthong = false;
	stressed_duration_factor = 1.0;
	stressed_amplitude_factor = 1.0;
	stressed_pitch_factor = 1.0;
}

Phone::~Phone()
{
	// Destructor logic if needed
}
void Phone::set_name(const String &p_name)
{
	name = p_name;
}
String Phone::get_name() const
{
	return name;
}
void Phone::set_phoneme_ipa(const String &p_ipa)
{
	phoneme_ipa = p_ipa;
}
String Phone::get_phoneme_ipa() const
{
	return phoneme_ipa;
}
void Phone::set_phoneme_variant(const String &p_variant)
{
	phoneme_variant = p_variant;
}
String Phone::get_phoneme_variant() const
{
	return phoneme_variant;
}
void Phone::set_is_diphthong(bool p_is_diphthong)
{
	is_diphthong = p_is_diphthong;
}
bool Phone::get_is_diphthong() const
{
	return is_diphthong;
}
void Phone::set_start_lpc_indices(const TypedArray<int> &p_indices)
{
	start_lpc_indices = p_indices;
}
TypedArray<int> Phone::get_start_lpc_indices() const
{
	return start_lpc_indices;
}
void Phone::set_end_lpc_indices(const TypedArray<int> &p_indices)
{
	end_lpc_indices = p_indices;
}
TypedArray<int> Phone::get_end_lpc_indices() const
{
	return end_lpc_indices;
}
void Phone::set_transition_curve(const TypedArray<double> &p_curve)
{
	transition_curve = p_curve;
}
TypedArray<double> Phone::get_transition_curve() const
{
	return transition_curve;
}
void Phone::set_attack_time(double p_time)
{
	attack_time = p_time;
}
double Phone::get_attack_time() const
{
	return attack_time;
}
void Phone::set_release_time(double p_time)
{
	release_time = p_time;
}
double Phone::get_release_time() const
{
	return release_time;
}
void Phone::set_stressed_duration_factor(double p_factor)
{
	stressed_duration_factor = p_factor;
}
double Phone::get_stressed_duration_factor() const
{
	return stressed_duration_factor;
}
void Phone::set_stressed_amplitude_factor(float p_factor)
{
	stressed_amplitude_factor = p_factor;
}
double Phone::get_stressed_amplitude_factor() const
{
	return stressed_amplitude_factor;
}
void Phone::set_stressed_pitch_factor(double p_factor)
{
	stressed_pitch_factor = p_factor;
}
double Phone::get_stressed_pitch_factor() const
{
	return stressed_pitch_factor;
}
void Phone::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_name", "name"), &Phone::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &Phone::get_name);

	ClassDB::bind_method(D_METHOD("set_phoneme_ipa", "ipa"), &Phone::set_phoneme_ipa);
	ClassDB::bind_method(D_METHOD("get_phoneme_ipa"), &Phone::get_phoneme_ipa);

	ClassDB::bind_method(D_METHOD("set_phoneme_variant", "variant"), &Phone::set_phoneme_variant);
	ClassDB::bind_method(D_METHOD("get_phoneme_variant"), &Phone::get_phoneme_variant);

	ClassDB::bind_method(D_METHOD("set_is_diphthong", "is_diphthong"), &Phone::set_is_diphthong);
	ClassDB::bind_method(D_METHOD("get_is_diphthong"), &Phone::get_is_diphthong);

	ClassDB::bind_method(D_METHOD("set_start_lpc_indices", "indices"), &Phone::set_start_lpc_indices);
	ClassDB::bind_method(D_METHOD("get_start_lpc_indices"), &Phone::get_start_lpc_indices);

	ClassDB::bind_method(D_METHOD("set_end_lpc_indices", "indices"), &Phone::set_end_lpc_indices);
	ClassDB::bind_method(D_METHOD("get_end_lpc_indices"), &Phone::get_end_lpc_indices);

	ClassDB::bind_method(D_METHOD("set_transition_curve", "curve"), &Phone::set_transition_curve);
	ClassDB::bind_method(D_METHOD("get_transition_curve"), &Phone::get_transition_curve);

	ClassDB::bind_method(D_METHOD("set_attack_time", "time"), &Phone::set_attack_time);
	ClassDB::bind_method(D_METHOD("get_attack_time"), &Phone::get_attack_time);

	ClassDB::bind_method(D_METHOD("set_release_time", "time"), &Phone::set_release_time);
	ClassDB::bind_method(D_METHOD("get_release_time"), &Phone::get_release_time);

	ClassDB::bind_method(D_METHOD("set_stressed_duration_factor", "factor"), &Phone::set_stressed_duration_factor);
	ClassDB::bind_method(D_METHOD("get_stressed_duration_factor"), &Phone::get_stressed_duration_factor);

	ClassDB::bind_method(D_METHOD("set_stressed_amplitude_factor", "factor"), &Phone::set_stressed_amplitude_factor);
	ClassDB::bind_method(D_METHOD("get_stressed_amplitude_factor"), &Phone::get_stressed_amplitude_factor);

	ClassDB::bind_method(D_METHOD("set_stressed_pitch_factor", "factor"), &Phone::set_stressed_pitch_factor);
	ClassDB::bind_method(D_METHOD("get_stressed_pitch_factor"), &Phone::get_stressed_pitch_factor);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "phoneme_ipa"), "set_phoneme_ipa", "get_phoneme_ipa");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "phoneme_variant"), "set_phoneme_variant", "get_phoneme_variant");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_diphthong"), "set_is_diphthong", "get_is_diphthong");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "start_lpc_indices", PROPERTY_HINT_ARRAY_TYPE, "int"), "set_start_lpc_indices", "get_start_lpc_indices");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "end_lpc_indices", PROPERTY_HINT_ARRAY_TYPE, "int"), "set_end_lpc_indices", "get_end_lpc_indices");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "transition_curve", PROPERTY_HINT_ARRAY_TYPE, "float"), "set_transition_curve", "get_transition_curve");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_time"), "set_attack_time", "get_attack_time");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "release_time"), "set_release_time", "get_release_time");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "stressed_duration_factor"), "set_stressed_duration_factor", "get_stressed_duration_factor");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "stressed_amplitude_factor"), "set_stressed_amplitude_factor", "get_stressed_amplitude_factor");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "stressed_pitch_factor"), "set_stressed_pitch_factor", "get_stressed_pitch_factor");
}
