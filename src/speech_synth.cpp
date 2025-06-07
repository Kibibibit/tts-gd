#include "speech_synth.h"
#include "wave_generator.h"
#include "biquad_filter.h"
#include "phoneme.h"

using namespace godot;
SpeechSynth::SpeechSynth() {
	this->sample_rate = 44100.0;
	this->current_phoneme_index = 0;
	this->phoneme_count = 0;
	this->finished = false;
	this->voiced_ampltiude = 1.0;
	this->voiced_shape = 0.1;
	this->fundamental_frequency = 120.0;
	this->voiced_phase = 0.0;

	this->wave_generator = Ref<WaveGenerator>(memnew(WaveGenerator()));


	this->formant_filters.resize(5);
	this->antiformant_filters.resize(2);

	for (int i = 0; i < this->formant_filters.size(); ++i) {
		this->formant_filters.set(i, Ref<BiquadFilter>(memnew(BiquadFilter())));
	}
	for (int i = 0; i < this->antiformant_filters.size(); ++i) {
		this->antiformant_filters.set(i, Ref<BiquadFilter>(memnew(BiquadFilter())));
	}

	this->formant_filter_weights.resize(5);
	this->antiformant_filter_weights.resize(2);
	for (int i = 0; i < this->formant_filter_weights.size(); ++i) {
		this->formant_filter_weights.set(i, 1.0);
	}
	for (int i = 0; i < this->antiformant_filter_weights.size(); ++i) {
		this->antiformant_filter_weights.set(i, 1.0);
	}
}

SpeechSynth::~SpeechSynth() {
	// Destructor logic if needed
}


void SpeechSynth::set_sample_rate(double sample_rate) {
	this->sample_rate = sample_rate;
	for (int i = 0; i < this->formant_filters.size(); ++i) {
		Ref<BiquadFilter> filter = this->formant_filters.get(i);
		filter->set_sample_rate(sample_rate);
	}
	for (int i = 0; i < this->antiformant_filters.size(); ++i) {
		Ref<BiquadFilter> filter = this->antiformant_filters.get(i);
		filter->set_sample_rate(sample_rate);
	}
}

double SpeechSynth::get_sample_rate() const {
	return sample_rate;
}
void SpeechSynth::set_wave_generator(const Ref<WaveGenerator> &wave_generator) {
	this->wave_generator = wave_generator;
}
Ref<WaveGenerator> SpeechSynth::get_wave_generator() const {
	return wave_generator;
}

void SpeechSynth::set_voiced_shape(double shape) {
	voiced_shape = shape;
}
double SpeechSynth::get_voiced_shape() const {
	return voiced_shape;
}
void SpeechSynth::set_voiced_amplitude(double amplitude) {
	voiced_ampltiude = amplitude;
}
double SpeechSynth::get_voiced_amplitude() const {
	return voiced_ampltiude;
}
int32_t SpeechSynth::get_phoneme_count() const {
	return phoneme_count;
}
int32_t SpeechSynth::get_current_phoneme_index() const {
	return current_phoneme_index;
}
void SpeechSynth::set_fundamental_frequency(double frequency) {
	fundamental_frequency = frequency;
}
double SpeechSynth::get_fundamental_frequency() const {
	return fundamental_frequency;
}

void SpeechSynth::set_phonemes_to_process(Array phonemes) {
	UtilityFunctions::print("Setting phonemes to process: ", phonemes.size());
	phonemes_to_process = phonemes;
	phoneme_count = phonemes.size();
	current_phoneme_index = 0;
	current_phoneme_time = 0.0;
	finished = false;
}

bool SpeechSynth::is_finished() const {
	return finished;
}
void SpeechSynth::stop() {
	finished = true;
	voiced_phase = 0.0; // Reset phase
	current_phoneme_index = phoneme_count; // Set to the end to stop processing
	current_phoneme_time = 0.0; // Reset time
}

double SpeechSynth::get_next_sample() {
	if (finished || current_phoneme_index >= phoneme_count || phonemes_to_process.size() == 0) {
		return 0.0;
	}

	Ref<Phoneme> current_phoneme = phonemes_to_process.get(current_phoneme_index);

	if (current_phoneme.is_null()) {
		UtilityFunctions::push_error("Current phoneme is null, cannot process.");
		finished = true; // Stop processing if phoneme is null
		return 0.0; // No phoneme to process
	}

	Ref<FormantSet> formant_set = current_phoneme->get_effective_formant_set(current_phoneme_time);


	for (int i = 0; i < formant_filters.size(); i++) {
		if (i < formant_set->get_formant_count()) {
			double frequency = formant_set->get_formant_frequency(i);
			double bandwidth = formant_set->get_formant_bandwidth(i);
			double gain = formant_set->get_formant_gain(i);
			Ref<BiquadFilter> filter = formant_filters.get(i);
			filter->set_frequency(frequency);
			filter->set_q(bandwidth);
			filter->set_gain(gain);
			filter->set_notch_filter(false);
			formant_filter_weights.set(i, 1.0);
		} else {
			formant_filter_weights.set(i, 0.0);
		}
	}

	for (int i = 0; i < antiformant_filters.size(); i++) {
		if (i < formant_set->get_antiformant_count()) {
			double frequency = formant_set->get_antiformant_frequency(i);
			double bandwidth = formant_set->get_antiformant_bandwidth(i);
			Ref<BiquadFilter> filter = antiformant_filters.get(i);
			filter->set_frequency(frequency);
			filter->set_q(bandwidth);
			filter->set_gain(0.0);
			filter->set_notch_filter(true);
			antiformant_filter_weights.set(i, 1.0);
		} else {
			antiformant_filter_weights.set(i, 0.0);
		}
	}

	double amplitude = current_phoneme->get_envelope()->amplitude_at(current_phoneme_time);
	double voice_mix = current_phoneme->get_voice_mix_ratio();
	double voiced_strength = voice_mix * voiced_ampltiude;
	double unvoiced_strength = (1.0 - voice_mix);


	double voiced_sample = wave_generator->voiced_wave(voiced_phase, voiced_shape) * voiced_strength;
	double unvoiced_sample = wave_generator->next_pink_noise() * unvoiced_strength;

	double sample = voiced_sample + unvoiced_sample;
	double result = 0.0;

	for (int i = 0; i < formant_filters.size(); i++) {
		Ref<BiquadFilter> filter = formant_filters.get(i);
		result += filter->process_signal(sample) * formant_filter_weights.get(i);
	}
	for (int i = 0; i < antiformant_filters.size(); i++) {
		Ref<BiquadFilter> filter = antiformant_filters.get(i);
		result -= filter->process_signal(sample) * antiformant_filter_weights.get(i);
	}

	double phase_increment = (current_phoneme->get_f0_modifier() * fundamental_frequency) / sample_rate;

	voiced_phase += phase_increment;
	current_phoneme_time += 1.0 / sample_rate;
	if (voiced_phase >= 1.0) {
		voiced_phase -= 1.0;
	}

	if (current_phoneme_time >= current_phoneme->get_typical_duration()) {
		current_phoneme_index++;
		current_phoneme_time = 0.0;
		if (current_phoneme_index >= phoneme_count) {
			finished = true;
			current_phoneme_index = 0;
			current_phoneme_time = 0.0;
			voiced_phase = 0.0;

		}
	}

	return result * amplitude;
	
	
}

PackedFloat32Array SpeechSynth::get_next_n_samples(int32_t n) {
	PackedFloat32Array samples;
	samples.resize(n);
	int32_t samples_added = 0;
	for (int i = 0; i < n; i++) {
		if (finished) {
			break;
		}
		samples_added++;
		samples.set(i, get_next_sample());
	}
	samples.resize(samples_added);
	return samples;
}


void SpeechSynth::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sample_rate", "sample_rate"), &SpeechSynth::set_sample_rate);
	ClassDB::bind_method(D_METHOD("get_sample_rate"), &SpeechSynth::get_sample_rate);

	ClassDB::bind_method(D_METHOD("set_wave_generator", "wave_generator"), &SpeechSynth::set_wave_generator);
	ClassDB::bind_method(D_METHOD("get_wave_generator"), &SpeechSynth::get_wave_generator);

	ClassDB::bind_method(D_METHOD("set_voiced_shape", "shape"), &SpeechSynth::set_voiced_shape);
	ClassDB::bind_method(D_METHOD("get_voiced_shape"), &SpeechSynth::get_voiced_shape);

	ClassDB::bind_method(D_METHOD("set_voiced_amplitude", "amplitude"), &SpeechSynth::set_voiced_amplitude);
	ClassDB::bind_method(D_METHOD("get_voiced_amplitude"), &SpeechSynth::get_voiced_amplitude);

	ClassDB::bind_method(D_METHOD("get_phoneme_count"), &SpeechSynth::get_phoneme_count);
	ClassDB::bind_method(D_METHOD("get_current_phoneme_index"), &SpeechSynth::get_current_phoneme_index);

	ClassDB::bind_method(D_METHOD("set_phonemes_to_process", "phonemes"), &SpeechSynth::set_phonemes_to_process);

	ClassDB::bind_method(D_METHOD("get_next_sample"), &SpeechSynth::get_next_sample);

	ClassDB::bind_method(D_METHOD("set_fundamental_frequency", "frequency"), &SpeechSynth::set_fundamental_frequency);
	ClassDB::bind_method(D_METHOD("get_fundamental_frequency"), &SpeechSynth::get_fundamental_frequency);

	ClassDB::bind_method(D_METHOD("is_finished"), &SpeechSynth::is_finished);
	ClassDB::bind_method(D_METHOD("stop"), &SpeechSynth::stop);

	ClassDB::bind_method(D_METHOD("get_next_n_samples", "n"), &SpeechSynth::get_next_n_samples);
}