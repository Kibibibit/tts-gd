
#include "envelope.h"

using namespace godot;

Envelope::Envelope() :
	attack_time(0.1),
	decay_time(0.1),
	sustain_level(0.8),
	release_time(0.1) {
}

Envelope::~Envelope() {
}

double Envelope::get_total_time() const {
	return attack_time + decay_time + sustain_time + release_time; 
}

double Envelope::amplitude_at(double time) const {
	if (time < 0.0) {
		return 0.0;
	}

	double release_start = attack_time + decay_time + sustain_time;
	double sustain_start = attack_time + decay_time;

	if (time < attack_time) {
		return time / attack_time; 
	} else if (time < sustain_start) {
		double decay_progress = (time - attack_time) / decay_time;
		return 1.0 - decay_progress * (1.0 - sustain_level);
	} else if (time < release_start) {
		return sustain_level;
	} else if (time < release_start + release_time) {
		double release_progress = (time - release_start) / release_time;
		return sustain_level * (1.0 - release_progress);
	} else {
		return 0.0;
	}
	
}


void Envelope::set_attack_time(double attack_time) {
	this->attack_time = attack_time;
}
double Envelope::get_attack_time() const {
	return attack_time;
}
void Envelope::set_decay_time(double decay_time) {
	this->decay_time = decay_time;
}
double Envelope::get_decay_time() const {
	return decay_time;
}
void Envelope::set_sustain_level(double sustain_level) {
	this->sustain_level = sustain_level;
}
double Envelope::get_sustain_level() const {
	return sustain_level;
}
void Envelope::set_release_time(double release_time) {
	this->release_time = release_time;
}
double Envelope::get_release_time() const {
	return release_time;
}
void Envelope::set_sustain_time(double sustain_time) {
	this->sustain_time = sustain_time;
}
double Envelope::get_sustain_time() const {
	return sustain_time;
}

void Envelope::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_total_time"), &Envelope::get_total_time);
	ClassDB::bind_method(D_METHOD("amplitude_at", "time"), &Envelope::amplitude_at);

	ClassDB::bind_method(D_METHOD("set_attack_time", "attack_time"), &Envelope::set_attack_time);
	ClassDB::bind_method(D_METHOD("get_attack_time"), &Envelope::get_attack_time);
	ClassDB::bind_method(D_METHOD("set_decay_time", "decay_time"), &Envelope::set_decay_time);
	ClassDB::bind_method(D_METHOD("get_decay_time"), &Envelope::get_decay_time);
	ClassDB::bind_method(D_METHOD("set_sustain_level", "sustain_level"), &Envelope::set_sustain_level);
	ClassDB::bind_method(D_METHOD("get_sustain_level"), &Envelope::get_sustain_level);
	ClassDB::bind_method(D_METHOD("set_release_time", "release_time"), &Envelope::set_release_time);
	ClassDB::bind_method(D_METHOD("get_release_time"), &Envelope::get_release_time);
	ClassDB::bind_method(D_METHOD("set_sustain_time", "sustain_time"), &Envelope::set_sustain_time);
	ClassDB::bind_method(D_METHOD("get_sustain_time"), &Envelope::get_sustain_time);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sustain_time", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_sustain_time", "get_sustain_time");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_time", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_attack_time", "get_attack_time");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "decay_time", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_decay_time", "get_decay_time");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sustain_level", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_sustain_level", "get_sustain_level");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "release_time", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_release_time", "get_release_time");
}