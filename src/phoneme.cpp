#include "phoneme.h"
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
Phoneme::Phoneme() {
}
Phoneme::~Phoneme() {
}
void Phoneme::set_full_name(const String &name) {
	this->full_name = name;
	if (this->full_name.length() == 3) {
		this->is_vowel = true;
	} else {
		this->is_vowel = false;
	}
	_parse_full_name();
	_validate();
}
String Phoneme::get_full_name() const {
	return this->full_name;
}
void Phoneme::set_stress(int stress) {
	this->stress = stress;
	if (this->stress < 0 || this->stress > 2) {
		UtilityFunctions::push_error("Phoneme stress must be between 0 and 2.");
		return;
	}
}
int Phoneme::get_stress() const {
	return this->stress;
}
void Phoneme::set_phoneme_name(const String &name) {
	this->phoneme_name = name;
}
String Phoneme::get_phoneme_name() const {
	return this->phoneme_name;
}
void Phoneme::_parse_full_name() {
	// Phonemes are in the format [A-Z]{1,2}[0-9]{0,1}
	// Where the first two characters are the phoneme name and the number is the stress.
	// Stress cannot be less than 0 and ideally not greater than 2.
	if (this->full_name.is_empty()) {
		UtilityFunctions::push_error("Phoneme full name cannot be empty.");
		return;
	}
	String name_str = "";
	String stress_str = "";
	for (int i = 0; i < this->full_name.length(); i++) {
		String c = "";
		c += this->full_name[i];
		if (c.is_valid_int()) {
			stress_str += this->full_name[i];
		} else {
			name_str += this->full_name[i];
		}
	}
	if (name_str.is_empty()) {
		UtilityFunctions::push_error("Phoneme name cannot be empty.");
		return;
	}
	this->phoneme_name = name_str;
	if (stress_str.is_empty()) {
		this->stress = 0;
	} else {
		this->stress = stress_str.to_int();
		if (this->stress < 0 || this->stress > 2) {
			UtilityFunctions::push_error("Phoneme stress must be between 0 and 2.");
			return;
		}
	}
}

bool Phoneme::_validate() const {
	if (this->full_name.is_empty()) {
		UtilityFunctions::push_error("Phoneme full name cannot be empty.");
		return false;
	}
	if (this->phoneme_name.is_empty()) {
		UtilityFunctions::push_error("Phoneme name cannot be empty.");
		return false;
	}
	if (this->stress < 0 || this->stress > 2) {
		UtilityFunctions::push_error("Phoneme stress must be between 0 and 2.");
		return false;
	}
	return true;
}

String Phoneme::_to_string() const {
	return this->full_name;
}
int Phoneme::_hash() const {
	return this->full_name.hash();
}

bool Phoneme::get_is_vowel() const {
	return this->is_vowel;
}
void Phoneme::set_is_vowel(bool is_vowel) {
	this->is_vowel = is_vowel;
	if (this->full_name.length() == 3) {
		this->is_vowel = true;
	} else {
		this->is_vowel = false;
	}
}

void Phoneme::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_full_name", "name"), &Phoneme::set_full_name);
	ClassDB::bind_method(D_METHOD("get_full_name"), &Phoneme::get_full_name);
	ClassDB::bind_method(D_METHOD("set_stress", "stress"), &Phoneme::set_stress);
	ClassDB::bind_method(D_METHOD("get_stress"), &Phoneme::get_stress);
	ClassDB::bind_method(D_METHOD("set_phoneme_name", "name"), &Phoneme::set_phoneme_name);
	ClassDB::bind_method(D_METHOD("get_phoneme_name"), &Phoneme::get_phoneme_name);
	ClassDB::bind_method(D_METHOD("get_is_vowel"), &Phoneme::get_is_vowel);
	ClassDB::bind_method(D_METHOD("set_is_vowel", "is_vowel"), &Phoneme::set_is_vowel);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "full_name"), "set_full_name", "get_full_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "stress", PROPERTY_HINT_RANGE, "0,2,1"), "set_stress", "get_stress");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "phoneme_name"), "set_phoneme_name", "get_phoneme_name");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_vowel"), "set_is_vowel", "get_is_vowel");
}

