
#include "formant_set.h"

using namespace godot;


FormantSet::FormantSet() {
	frequencies = PackedFloat32Array();
	bandwidths = PackedFloat32Array();
	gains = PackedFloat32Array();
	antiformant_frequencies = PackedFloat32Array();
	antiformant_bandwidths = PackedFloat32Array();
}
FormantSet::~FormantSet() {
}
void FormantSet::set_formant_data(const PackedFloat32Array &frequencies, const PackedFloat32Array &bandwidths, const PackedFloat32Array &gains) {
	this->frequencies = frequencies;
	this->bandwidths = bandwidths;
	this->gains = gains;
}
void FormantSet::set_antiformant_data(const PackedFloat32Array &frequencies, const PackedFloat32Array &bandwidths) {
	this->antiformant_frequencies = frequencies;
	this->antiformant_bandwidths = bandwidths;
}
void FormantSet::set_formant(int index, double frequency, double bandwidth, double gain) {
	DEV_ASSERT(index >= 0 && index < frequencies.size());
	if (index < 0 || index >= frequencies.size()) {
		return; // Index out of bounds
	}
	frequencies.set(index, frequency);
	bandwidths.set(index, bandwidth);
	gains.set(index, gain);
}
double FormantSet::get_formant_frequency(int index) const {
	DEV_ASSERT(index >= 0 && index < frequencies.size());
	if (index < 0 || index >= frequencies.size()) {
		return 0.0; // Index out of bounds
	}
	return frequencies.get(index);
}
double FormantSet::get_formant_bandwidth(int index) const {
	DEV_ASSERT(index >= 0 && index < bandwidths.size());
	if (index < 0 || index >= bandwidths.size()) {
		return 0.0; // Index out of bounds
	}
	return bandwidths.get(index);
}
double FormantSet::get_formant_gain(int index) const {
	DEV_ASSERT(index >= 0 && index < gains.size());
	if (index < 0 || index >= gains.size()) {
		return 0.0; // Index out of bounds
	}
	return gains.get(index);
}
void FormantSet::set_formant_frequency(int index, double frequency) {
	DEV_ASSERT(index >= 0 && index < frequencies.size());
	if (index < 0 || index >= frequencies.size()) {
		return; // Index out of bounds
	}
	frequencies.set(index, frequency);
}
void FormantSet::set_formant_bandwidth(int index, double bandwidth) {
	DEV_ASSERT(index >= 0 && index < bandwidths.size());
	if (index < 0 || index >= bandwidths.size()) {
		return; // Index out of bounds
	}
	bandwidths.set(index, bandwidth);
}
void FormantSet::set_formant_gain(int index, double gain) {
	DEV_ASSERT(index >= 0 && index < gains.size());
	if (index < 0 || index >= gains.size()) {
		return; // Index out of bounds
	}
	gains.set(index, gain);
}
int FormantSet::get_formant_count() const {
	return frequencies.size();
}
void FormantSet::set_antiformant(int index, double frequency, double bandwidth) {
	DEV_ASSERT(index >= 0 && index < antiformant_frequencies.size());
	if (index < 0 || index >= antiformant_frequencies.size()) {
		return; // Index out of bounds
	}
	antiformant_frequencies.set(index, frequency);
	antiformant_bandwidths.set(index, bandwidth);
}
double FormantSet::get_antiformant_frequency(int index) const {
	DEV_ASSERT(index >= 0 && index < antiformant_frequencies.size());
	if (index < 0 || index >= antiformant_frequencies.size()) {
		return 0.0; // Index out of bounds
	}
	return antiformant_frequencies.get(index);
}
double FormantSet::get_antiformant_bandwidth(int index) const {
	DEV_ASSERT(index >= 0 && index < antiformant_bandwidths.size());
	if (index < 0 || index >= antiformant_bandwidths.size()) {
		return 0.0; // Index out of bounds
	}
	return antiformant_bandwidths.get(index);
}
void FormantSet::set_antiformant_frequency(int index, double frequency) {
	DEV_ASSERT(index >= 0 && index < antiformant_frequencies.size());
	if (index < 0 || index >= antiformant_frequencies.size()) {
		return; // Index out of bounds
	}
	antiformant_frequencies.set(index, frequency);
}
void FormantSet::set_antiformant_bandwidth(int index, double bandwidth) {
	DEV_ASSERT(index >= 0 && index < antiformant_bandwidths.size());
	if (index < 0 || index >= antiformant_bandwidths.size()) {
		return; // Index out of bounds
	}
	antiformant_bandwidths.set(index, bandwidth);
}
int FormantSet::get_antiformant_count() const {
	return antiformant_frequencies.size();
}
PackedFloat32Array FormantSet::get_frequencies() const {
	return frequencies;
}
PackedFloat32Array FormantSet::get_bandwidths() const {
	return bandwidths;
}
PackedFloat32Array FormantSet::get_gains() const {
	return gains;
}
PackedFloat32Array FormantSet::get_antiformant_frequencies() const {
	return antiformant_frequencies;
}
PackedFloat32Array FormantSet::get_antiformant_bandwidths() const {
	return antiformant_bandwidths;
}

Ref<FormantSet> FormantSet::blend(const Ref<FormantSet> &other, double t) const {
	Ref<FormantSet> result = memnew(FormantSet);
	int formant_count = get_formant_count();
	int antiformant_count = get_antiformant_count();

	PackedFloat32Array new_frequencies;
	PackedFloat32Array new_bandwidths;
	PackedFloat32Array new_gains;
	PackedFloat32Array new_antiformant_frequencies;
	PackedFloat32Array new_antiformant_bandwidths;

	new_frequencies.resize(formant_count);
	new_bandwidths.resize(formant_count);
	new_gains.resize(formant_count);
	new_antiformant_frequencies.resize(antiformant_count);
	new_antiformant_bandwidths.resize(antiformant_count);

	for (int i = 0; i < formant_count; i++) {
		new_frequencies.set(i, UtilityFunctions::lerp(this->get_formant_frequency(i), other->get_formant_frequency(i), t));
		new_bandwidths.set(i, UtilityFunctions::lerp(this->get_formant_bandwidth(i), other->get_formant_bandwidth(i), t));
		new_gains.set(i, UtilityFunctions::lerp(this->get_formant_gain(i), other->get_formant_gain(i), t));
	}

	for (int i = 0; i < antiformant_count; i++) {
		new_antiformant_frequencies.set(i, UtilityFunctions::lerp(this->get_antiformant_frequency(i), other->get_antiformant_frequency(i), t));
		new_antiformant_bandwidths.set(i, UtilityFunctions::lerp(this->get_antiformant_bandwidth(i), other->get_antiformant_bandwidth(i), t));
	}

	result->set_formant_data(new_frequencies, new_bandwidths, new_gains);
	result->set_antiformant_data(new_antiformant_frequencies, new_antiformant_bandwidths);

	return result;
}

void FormantSet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_formant_data", "frequencies", "bandwidths", "gains"), &FormantSet::set_formant_data);
	ClassDB::bind_method(D_METHOD("set_antiformant_data", "frequencies", "bandwidths"), &FormantSet::set_antiformant_data);
	ClassDB::bind_method(D_METHOD("set_formant", "index", "frequency", "bandwidth", "gain"), &FormantSet::set_formant);
	ClassDB::bind_method(D_METHOD("get_formant_frequency", "index"), &FormantSet::get_formant_frequency);
	ClassDB::bind_method(D_METHOD("get_formant_bandwidth", "index"), &FormantSet::get_formant_bandwidth);
	ClassDB::bind_method(D_METHOD("get_formant_gain", "index"), &FormantSet::get_formant_gain);
	ClassDB::bind_method(D_METHOD("set_formant_frequency", "index", "frequency"), &FormantSet::set_formant_frequency);
	ClassDB::bind_method(D_METHOD("set_formant_bandwidth", "index", "bandwidth"), &FormantSet::set_formant_bandwidth);
	ClassDB::bind_method(D_METHOD("set_formant_gain", "index", "gain"), &FormantSet::set_formant_gain);
	ClassDB::bind_method(D_METHOD("get_formant_count"), &FormantSet::get_formant_count);
	ClassDB::bind_method(D_METHOD("set_antiformant", "index", "frequency", "bandwidth"), &FormantSet::set_antiformant);
	ClassDB::bind_method(D_METHOD("get_antiformant_frequency", "index"), &FormantSet::get_antiformant_frequency);
	ClassDB::bind_method(D_METHOD("get_antiformant_bandwidth", "index"), &FormantSet::get_antiformant_bandwidth);
	ClassDB::bind_method(D_METHOD("set_antiformant_frequency", "index", "frequency"), &FormantSet::set_antiformant_frequency);
	ClassDB::bind_method(D_METHOD("set_antiformant_bandwidth", "index", "bandwidth"), &FormantSet::set_antiformant_bandwidth);
	ClassDB::bind_method(D_METHOD("get_antiformant_count"), &FormantSet::get_antiformant_count);
	ClassDB::bind_method(D_METHOD("blend", "other", "t"), &FormantSet::blend, DEFVAL(0.5));
}