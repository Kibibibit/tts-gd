#include "lpc_synthesizer.h"
#include "lpc_file.h"


using namespace godot;

LPCSynthesizer::LPCSynthesizer() {
	history = PackedFloat32Array();
	history.resize(max_order);
	wave_generator = Ref<WaveGenerator>();
	reset();
}
LPCSynthesizer::~LPCSynthesizer() {
	// Destructor logic if needed
}

void LPCSynthesizer::reset() {
	reset_history();
	reset_phase();
	prev_gain = 0.0;
	prev_excitation = 0.0;
	progress = 0.0;
}

void LPCSynthesizer::reset_history() {
	history.fill(0.0f);
}
void LPCSynthesizer::reset_phase() {
	wave_phase = 0.0;
}

PackedFloat32Array LPCSynthesizer::get_history() const {
	return history;
}
void LPCSynthesizer::set_lpc_file(const Ref<LPCFile> &file) {
	lpc_file = file;
	phase_increment = base_frequency / lpc_file->get_sample_rate(); 
}
Ref<LPCFile> LPCSynthesizer::get_lpc_file() const {
	return lpc_file;
}
void LPCSynthesizer::set_max_order(int order) {
	max_order = order;
	history.resize(max_order);
}
int LPCSynthesizer::get_max_order() const {
	return max_order;
}

void LPCSynthesizer::set_wave_generator(const Ref<WaveGenerator> &generator) {
	wave_generator = generator;
	reset_phase();
}
Ref<WaveGenerator> LPCSynthesizer::get_wave_generator() const {
	return wave_generator;
}


void LPCSynthesizer::set_progress(double p_progress) {
	progress = p_progress;
}

double LPCSynthesizer::get_progress() const {
	return progress;
}

void LPCSynthesizer::set_base_frequency(double frequency) {
	if (frequency <= 0.0) {
		UtilityFunctions::push_error("Base frequency must be greater than 0.0", __FILE__, __LINE__);
		return;
	}
	base_frequency = frequency;
	phase_increment = base_frequency / lpc_file->get_sample_rate();
}
double LPCSynthesizer::get_base_frequency() const {
	return base_frequency;
}
void LPCSynthesizer::set_wave_shape(double shape) {
	if (shape < 0.0 || shape > 1.0) {
		UtilityFunctions::push_error("Wave shape must be between 0.0 and 1.0", __FILE__, __LINE__);
		return;
	}
	wave_shape = shape;
}
double LPCSynthesizer::get_wave_shape() const {
	return wave_shape;
}

void LPCSynthesizer::set_wave_phase(double phase) {
	wave_phase = fmod(phase, 1.0);
}

double LPCSynthesizer::get_wave_phase() const {
	return wave_phase;
}

void LPCSynthesizer::set_consonant_upper_threshold(double threshold) {
	if (threshold < 0.0 || threshold > 1.0) {
		UtilityFunctions::push_error("Consonant upper threshold must be between 0.0 and 1.0", __FILE__, __LINE__);
		return;
	}
	consonant_upper_threshold = threshold;
}
double LPCSynthesizer::get_consonant_upper_threshold() const {
	return consonant_upper_threshold;
}
void LPCSynthesizer::set_consonant_lower_threshold(double threshold) {
	if (threshold < 0.0 || threshold > 1.0) {
		UtilityFunctions::push_error("Consonant lower threshold must be between 0.0 and 1.0", __FILE__, __LINE__);
		return;
	}
	consonant_lower_threshold = threshold;
}
double LPCSynthesizer::get_consonant_lower_threshold() const {
	return consonant_lower_threshold;
}
void LPCSynthesizer::set_consonant_pre_emphasis_factor(double factor) {
	if (factor < 0.0 || factor > 1.0) {
		UtilityFunctions::push_error("Consonant pre-emphasis factor must be between 0.0 and 1.0", __FILE__, __LINE__);
		return;
	}
	consonant_pre_emphasis_factor = factor;
}
double LPCSynthesizer::get_consonant_pre_emphasis_factor() const {
	return consonant_pre_emphasis_factor;
}
void LPCSynthesizer::set_voiced_gain_factor(double factor) {
	if (factor < 0.0) {
		UtilityFunctions::push_error("Voiced gain factor must be non-negative", __FILE__, __LINE__);
		return;
	}
	voiced_gain_factor = factor;
}
double LPCSynthesizer::get_voiced_gain_factor() const {
	return voiced_gain_factor;
}
void LPCSynthesizer::set_voiced_coeff_stability_factor(double factor) {
	if (factor < 0.0 || factor > 1.0) {
		UtilityFunctions::push_error("Voiced coefficient stability factor must be between 0.0 and 1.0", __FILE__, __LINE__);
		return;
	}
	voiced_coeff_stability_factor = factor;
}
double LPCSynthesizer::get_voiced_coeff_stability_factor() const {
	return voiced_coeff_stability_factor;
}
void LPCSynthesizer::set_breath(double breath) {
	if (breath < 0.0) {
		UtilityFunctions::push_error("Breath must be non-negative", __FILE__, __LINE__);
		return;
	}
	this->breath = breath;
}
double LPCSynthesizer::get_breath() const {
	return breath;
}
void LPCSynthesizer::set_interpolate_coefficients(bool interpolate) {
	interpolate_coefficients = interpolate;
}
bool LPCSynthesizer::get_interpolate_coefficients() const {
	return interpolate_coefficients;
}


double LPCSynthesizer::next_sample() {
	if (!lpc_file.is_valid()) {
		UtilityFunctions::push_error("Invalid LPC file", __FILE__, __LINE__);
		return 0.0; // Invalid state
	}
	if (max_order <= 0) {
		UtilityFunctions::push_error("Max order must be greater than 0", __FILE__, __LINE__);
		return 0.0; // Invalid state
	}
	if (!wave_generator.is_valid()) {
		UtilityFunctions::push_error("Wave generator is not set", __FILE__, __LINE__);
		return 0.0; // Invalid state
	}
	
	double frame_position = progress / lpc_file->get_dx();
	int frame_index = int(Math::floor(frame_position)) % lpc_file->get_frame_count();

	int next_frame_index = (frame_index + 1) % lpc_file->get_frame_count();
	double frame_progress = Math::fmod(frame_position, 1.0);


	const LPCFile::LPCFrame &frame = lpc_file->get_frames()[frame_index];
	const LPCFile::LPCFrame &next_frame = lpc_file->get_frames()[next_frame_index];


	double coeff_stability = 0.0;

	for (int i = 0; i < Math::min(max_order, frame.n_coefficients); i++) {
		coeff_stability += std::abs(frame.coefficients[i]);
	}

	double voiced_prob = std::min(1.0, std::max(0.0, (frame.gain * voiced_gain_factor) - (coeff_stability * voiced_coeff_stability_factor)));

	double gain = Math::lerp(frame.gain, next_frame.gain, frame_progress);

	double voiced_wave = wave_generator->voiced_wave(wave_phase, wave_shape);
	
	double unvoiced_wave = wave_generator->next_pink_noise() * breath;


	double gain_delta = std::abs(gain - prev_gain);

	bool likely_cosonant = (gain_delta > consonant_upper_threshold) || (gain_delta < consonant_lower_threshold);

	prev_gain = gain;


	double excitation = Math::lerp(voiced_wave, unvoiced_wave, voiced_prob);


	
	if (likely_cosonant) {
		// Apply pre-emphasis for consonants
		if (voiced_prob < 0.5) {
			excitation = excitation - (prev_excitation * consonant_pre_emphasis_factor);
		}
	}

	prev_excitation = excitation;

	double output = excitation;

	// Interpolate coefficients and gain
	for (int i = 0; i < max_order; ++i) {
		double current_coeff = frame.n_coefficients > i ? frame.coefficients[i] : 0.0;
		double next_coeff = next_frame.n_coefficients > i ? next_frame.coefficients[i] : 0.0;
		double coeff = Math::lerp(current_coeff, next_coeff, frame_progress);
		output -= coeff * history[i];
	}

	// Update history with the new sample
	for (int i = max_order - 1; i > 0; --i) {
		history[i] = history[i - 1];
	}

	history[0] = output;

	// Update phase and frame indicies
	wave_phase = Math::fmod(wave_phase + phase_increment, 1.0);

	progress = Math::fmod(progress+lpc_file->get_sampling_period(), lpc_file->get_x_max() - lpc_file->get_x_min());
	


	
	return output * std::sqrt(gain);
}

PackedFloat32Array LPCSynthesizer::next_n_samples(int n) {
	PackedFloat32Array samples;
	samples.resize(n);
	for (int i = 0; i < n; ++i) {
		samples[i] = next_sample();
	}
	return samples;
}

void LPCSynthesizer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("reset"), &LPCSynthesizer::reset);
	ClassDB::bind_method(D_METHOD("get_history"), &LPCSynthesizer::get_history);
	ClassDB::bind_method(D_METHOD("set_lpc_file", "file"), &LPCSynthesizer::set_lpc_file);
	ClassDB::bind_method(D_METHOD("get_lpc_file"), &LPCSynthesizer::get_lpc_file);
	ClassDB::bind_method(D_METHOD("set_max_order", "order"), &LPCSynthesizer::set_max_order);
	ClassDB::bind_method(D_METHOD("get_max_order"), &LPCSynthesizer::get_max_order);
	ClassDB::bind_method(D_METHOD("next_sample"), &LPCSynthesizer::next_sample);
	ClassDB::bind_method(D_METHOD("next_n_samples", "n"), &LPCSynthesizer::next_n_samples);
	ClassDB::bind_method(D_METHOD("set_wave_generator", "generator"), &LPCSynthesizer::set_wave_generator);
	ClassDB::bind_method(D_METHOD("get_wave_generator"), &LPCSynthesizer::get_wave_generator);
	ClassDB::bind_method(D_METHOD("set_base_frequency", "frequency"), &LPCSynthesizer::set_base_frequency);
	ClassDB::bind_method(D_METHOD("get_base_frequency"), &LPCSynthesizer::get_base_frequency);
	ClassDB::bind_method(D_METHOD("set_wave_shape", "shape"), &LPCSynthesizer::set_wave_shape);
	ClassDB::bind_method(D_METHOD("get_wave_shape"), &LPCSynthesizer::get_wave_shape);
	ClassDB::bind_method(D_METHOD("set_wave_phase", "phase"), &LPCSynthesizer::set_wave_phase);
	ClassDB::bind_method(D_METHOD("get_wave_phase"), &LPCSynthesizer::get_wave_phase);
	ClassDB::bind_method(D_METHOD("set_consonant_upper_threshold", "threshold"), &LPCSynthesizer::set_consonant_upper_threshold);
	ClassDB::bind_method(D_METHOD("get_consonant_upper_threshold"), &LPCSynthesizer::get_consonant_upper_threshold);
	ClassDB::bind_method(D_METHOD("set_consonant_lower_threshold", "threshold"), &LPCSynthesizer::set_consonant_lower_threshold);
	ClassDB::bind_method(D_METHOD("get_consonant_lower_threshold"), &LPCSynthesizer::get_consonant_lower_threshold);
	ClassDB::bind_method(D_METHOD("set_consonant_pre_emphasis_factor", "factor"), &LPCSynthesizer::set_consonant_pre_emphasis_factor);
	ClassDB::bind_method(D_METHOD("get_consonant_pre_emphasis_factor"), &LPCSynthesizer::get_consonant_pre_emphasis_factor);
	ClassDB::bind_method(D_METHOD("set_voiced_gain_factor", "factor"), &LPCSynthesizer::set_voiced_gain_factor);
	ClassDB::bind_method(D_METHOD("get_voiced_gain_factor"), &LPCSynthesizer::get_voiced_gain_factor);
	ClassDB::bind_method(D_METHOD("set_voiced_coeff_stability_factor", "factor"), &LPCSynthesizer::set_voiced_coeff_stability_factor);
	ClassDB::bind_method(D_METHOD("get_voiced_coeff_stability_factor"), &LPCSynthesizer::get_voiced_coeff_stability_factor);
	ClassDB::bind_method(D_METHOD("set_breath", "breath"), &LPCSynthesizer::set_breath);
	ClassDB::bind_method(D_METHOD("get_breath"), &LPCSynthesizer::get_breath);
	ClassDB::bind_method(D_METHOD("set_progress", "progress"), &LPCSynthesizer::set_progress);
	ClassDB::bind_method(D_METHOD("get_progress"), &LPCSynthesizer::get_progress);
	ClassDB::bind_method(D_METHOD("set_interpolate_coefficients", "interpolate"), &LPCSynthesizer::set_interpolate_coefficients);
	ClassDB::bind_method(D_METHOD("get_interpolate_coefficients"), &LPCSynthesizer::get_interpolate_coefficients);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lpc_file", PROPERTY_HINT_RESOURCE_TYPE, "LPCFile"), "set_lpc_file", "get_lpc_file");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_order", PROPERTY_HINT_RANGE, "1,1000,1"), "set_max_order", "get_max_order");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_frequency", PROPERTY_HINT_RANGE, "0.0,10000.0,0.01"), "set_base_frequency", "get_base_frequency");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wave_shape", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_wave_shape", "get_wave_shape");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "consonant_upper_threshold", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_consonant_upper_threshold", "get_consonant_upper_threshold");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "consonant_lower_threshold", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_consonant_lower_threshold", "get_consonant_lower_threshold");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "consonant_pre_emphasis_factor", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_consonant_pre_emphasis_factor", "get_consonant_pre_emphasis_factor");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "voiced_gain_factor", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_voiced_gain_factor", "get_voiced_gain_factor");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "voiced_coeff_stability_factor", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_voiced_coeff_stability_factor", "get_voiced_coeff_stability_factor");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "breath", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_breath", "get_breath");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "wave_generator", PROPERTY_HINT_RESOURCE_TYPE, "WaveGenerator"), "set_wave_generator", "get_wave_generator");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wave_phase", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_wave_phase", "get_wave_phase");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "progress", PROPERTY_HINT_RANGE, "0.0,10000.0,0.01"), "set_progress", "get_progress");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "interpolate_coefficients"), "set_interpolate_coefficients", "get_interpolate_coefficients");
}