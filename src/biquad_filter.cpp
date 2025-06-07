
#include "biquad_filter.h"

using namespace godot;


BiquadFilter::BiquadFilter() {
	sample_rate = 44100.0;
	x1 = x2 = y1 = y2 = 0.0;
	b0 = b1 = b2 = a1 = a2 = 0.0;
	frequency = 440.0; 
	q = 1.0; 
	gain = 0.0; 
}

BiquadFilter::~BiquadFilter() {}


void BiquadFilter::reset_state() {
	x1 = x2 = y1 = y2 = 0.0;
	b0 = b1 = b2 = a1 = a2 = 0.0;
}

void BiquadFilter::initialize(double sample_rate, bool notch, double frequency, double q, double gain) {
	this->sample_rate = sample_rate;
	this->frequency = frequency;
	this->q = q;
	this->gain = gain;
	this->is_notch = notch;
	_update_coefficients();
}

double BiquadFilter::process_signal(double input) {
	double output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
	x2 = x1;
	x1 = input;
	y2 = y1;
	y1 = output;
	return output;
}

void BiquadFilter::_update_coefficients() {
	// Convert from Hz to angular frequency -> so we can use trig functions on it
	double omega = 2.0 * M_PI * frequency / sample_rate;
	// This creates our bandwith (the width of the filter)
	double alpha = sin(omega) / (2.0 * q);
	// Converts gain from dB to linear scale

	double norm;
	double A = pow(10.0, gain / 40.0);

	if (this->is_notch) {
		double cos_omega = cos(omega);

		b0 = 1.0;
		b1 = -2.0 * cos_omega;
		b2 = 1.0;
		a1 = -2.0 * cos_omega;
		a2 = 1.0 - alpha;


		
	} else {
		
		b0 = alpha * A;
		b1 = 0.0;
		b2 = -alpha * A;
		a1 = -2.0 * cos(omega);
		a2 = 1.0 - alpha;
		

	}

	norm = 1.0 / (1.0 + alpha);
	b0 *= norm;
	b1 *= norm;
	b2 *= norm;
	a1 *= norm;
	a2 *= norm;
	
}


void BiquadFilter::set_sample_rate(double sample_rate) {
	this->sample_rate = sample_rate;
	_update_coefficients();
}
double BiquadFilter::get_sample_rate() const {
	return sample_rate;
}
void BiquadFilter::set_frequency(double frequency) {
	this->frequency = frequency;
	_update_coefficients();
}
double BiquadFilter::get_frequency() const {
	return frequency;
}
void BiquadFilter::set_q(double q) {
	this->q = q;
	_update_coefficients();
}
double BiquadFilter::get_q() const {
	return q;
}
void BiquadFilter::set_gain(double gain) {
	this->gain = gain;
	_update_coefficients();
}
double BiquadFilter::get_gain() const {
	return gain;
}
void BiquadFilter::set_a1(double a1) {
	this->a1 = a1;
}
double BiquadFilter::get_a1() const {
	return a1;
}
void BiquadFilter::set_a2(double a2) {
	this->a2 = a2;
}
double BiquadFilter::get_a2() const {
	return a2;
}
void BiquadFilter::set_b0(double b0) {
	this->b0 = b0;
}
double BiquadFilter::get_b0() const {
	return b0;
}
void BiquadFilter::set_b1(double b1) {
	this->b1 = b1;
}
double BiquadFilter::get_b1() const {
	return b1;
}
void BiquadFilter::set_b2(double b2) {
	this->b2 = b2;
}
double BiquadFilter::get_b2() const {
	return b2;
}

bool BiquadFilter::is_notch_filter() const {
	return is_notch;
}
void BiquadFilter::set_notch_filter(bool is_notch) {
	if (this->is_notch != is_notch) {
		this->reset_state(); // Reset state when changing filter type
	}
	this->is_notch = is_notch;
	_update_coefficients();
}

void BiquadFilter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("reset_state"), &BiquadFilter::reset_state);
	ClassDB::bind_method(D_METHOD("initialize", "sample_rate", "is_notch", "frequency", "q", "gain"), &BiquadFilter::initialize);
	ClassDB::bind_method(D_METHOD("process_signal", "input"), &BiquadFilter::process_signal);

	ClassDB::bind_method(D_METHOD("set_sample_rate", "sample_rate"), &BiquadFilter::set_sample_rate);
	ClassDB::bind_method(D_METHOD("get_sample_rate"), &BiquadFilter::get_sample_rate);
	ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &BiquadFilter::set_frequency);
	ClassDB::bind_method(D_METHOD("get_frequency"), &BiquadFilter::get_frequency);
	ClassDB::bind_method(D_METHOD("set_q", "q"), &BiquadFilter::set_q);
	ClassDB::bind_method(D_METHOD("get_q"), &BiquadFilter::get_q);
	ClassDB::bind_method(D_METHOD("set_gain", "gain"), &BiquadFilter::set_gain);
	ClassDB::bind_method(D_METHOD("get_gain"), &BiquadFilter::get_gain);

	ClassDB::bind_method(D_METHOD("set_a1", "a1"), &BiquadFilter::set_a1);
	ClassDB::bind_method(D_METHOD("get_a1"), &BiquadFilter::get_a1);
	ClassDB::bind_method(D_METHOD("set_a2", "a2"), &BiquadFilter::set_a2);
	ClassDB::bind_method(D_METHOD("get_a2"), &BiquadFilter::get_a2);
	ClassDB::bind_method(D_METHOD("set_b0", "b0"), &BiquadFilter::set_b0);
	ClassDB::bind_method(D_METHOD("get_b0"), &BiquadFilter::get_b0);
	ClassDB::bind_method(D_METHOD("set_b1", "b1"), &BiquadFilter::set_b1);
	ClassDB::bind_method(D_METHOD("get_b1"), &BiquadFilter::get_b1);
	ClassDB::bind_method(D_METHOD("set_b2", "b2"), &BiquadFilter::set_b2);
	ClassDB::bind_method(D_METHOD("get_b2"), &BiquadFilter::get_b2);
	ClassDB::bind_method(D_METHOD("is_notch_filter"), &BiquadFilter::is_notch_filter);
	ClassDB::bind_method(D_METHOD("set_notch_filter", "is_notch"), &BiquadFilter::set_notch_filter);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sample_rate", PROPERTY_HINT_RANGE, "1,96000,1"), "set_sample_rate", "get_sample_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency", PROPERTY_HINT_RANGE, "0.1,20000,0.1"), "set_frequency", "get_frequency");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "q", PROPERTY_HINT_RANGE, "0.1,10,0.1"), "set_q", "get_q");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PROPERTY_HINT_RANGE, "-24,24,0.1"), "set_gain", "get_gain");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "a1", PROPERTY_HINT_RANGE, "-1,1,0.01"), "set_a1", "get_a1");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "a2", PROPERTY_HINT_RANGE, "-1,1,0.01"), "set_a2", "get_a2");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "b0", PROPERTY_HINT_RANGE, "-1,1,0.01"), "set_b0", "get_b0");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "b1", PROPERTY_HINT_RANGE, "-1,1,0.01"), "set_b1", "get_b1");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "b2", PROPERTY_HINT_RANGE, "-1,1,0.01"), "set_b2", "get_b2");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_notch_filter"), "set_notch_filter", "is_notch_filter");
};