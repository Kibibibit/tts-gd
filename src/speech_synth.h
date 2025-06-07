#ifndef TTS_GD_SPEECH_SYNTH_H
#define TTS_GD_SPEECH_SYNTH_H

#include "wave_generator.h"
#include "biquad_filter.h"
#include "phoneme.h"
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot
{

	class SpeechSynth : public RefCounted
	{
		GDCLASS(SpeechSynth, RefCounted);

	private:
		Ref<WaveGenerator> wave_generator;
		Array formant_filters;
		Array antiformant_filters;
		PackedFloat32Array formant_filter_weights;
		PackedFloat32Array antiformant_filter_weights;

		Array phonemes_to_process;
		int32_t current_phoneme_index;
		int32_t phoneme_count;
		double sample_rate;
		double current_phoneme_time;
		double voiced_shape;
		double voiced_phase;
		double voiced_ampltiude;

		double fundamental_frequency;

		bool finished;

	protected:
		static void _bind_methods();

	public:
		SpeechSynth();
		~SpeechSynth();

		void set_sample_rate(double sample_rate);
		double get_sample_rate() const;

		void set_wave_generator(const Ref<WaveGenerator> &wave_generator);
		Ref<WaveGenerator> get_wave_generator() const;

		void set_voiced_shape(double shape);
		double get_voiced_shape() const;

		void set_voiced_amplitude(double amplitude);
		double get_voiced_amplitude() const;

		int32_t get_phoneme_count() const;
		int32_t get_current_phoneme_index() const;

		void set_phonemes_to_process(Array phonemes);

		double get_next_sample();
		PackedFloat32Array get_next_n_samples(int32_t n);

		double get_fundamental_frequency() const;
		void set_fundamental_frequency(double frequency);

		bool is_finished() const;

		void stop();
	};
}

#endif // TTS_GD_SPEECH_SYNTH_H
