#ifndef TTS_GD_LPC_SYNTHESIZER_H
#define TTS_GD_LPC_SYNTHESIZER_H

#include "lpc_file.h"
#include "wave_generator.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/typed_array.hpp>


namespace godot {
class LPCSynthesizer : public RefCounted {
	GDCLASS(LPCSynthesizer, RefCounted);


	private:
		PackedFloat32Array history;
		Ref<LPCFile> lpc_file;
		double wave_phase = 0.0;
		int max_order = 16; // Default LPC order
		double prev_gain = 0.0;
		double prev_excitation = 0.0;
		double phase_increment = 0.0;
		double base_frequency = 100.0;
		double wave_shape = 0.0;
		Ref<WaveGenerator> wave_generator;
		double consonant_upper_threshold = 0.2;
		double consonant_lower_threshold = 0.05;
		double consonant_pre_emphasis_factor = 0.95;
		double voiced_gain_factor = 10.0;
		double voiced_coeff_stability_factor = 0.5;
		double breath = 2.0;
		double progress = 0.0;
		bool interpolate_coefficients = true;

	
	protected:
		static void _bind_methods();
	public:
		LPCSynthesizer();
		~LPCSynthesizer();

		void reset_history();
		void reset_phase();
		void reset();
		PackedFloat32Array get_history() const;

		double get_progress() const;
		void set_progress(double p_progress);

		double next_sample();
		PackedFloat32Array next_n_samples(int n);


		// Getters and Setters
		void set_lpc_file(const Ref<LPCFile> &file);
		Ref<LPCFile> get_lpc_file() const;

		void set_max_order(int order);
		int get_max_order() const;

		void set_wave_generator(const Ref<WaveGenerator> &generator);
		Ref<WaveGenerator> get_wave_generator() const;

		double get_base_frequency() const;
		void set_base_frequency(double frequency);

		double get_wave_shape() const;
		void set_wave_shape(double shape);

		double get_wave_phase() const;
		void set_wave_phase(double phase);

		double get_consonant_upper_threshold() const;
		void set_consonant_upper_threshold(double threshold);
		double get_consonant_lower_threshold() const;
		void set_consonant_lower_threshold(double threshold);

		double get_consonant_pre_emphasis_factor() const;
		void set_consonant_pre_emphasis_factor(double factor);
		double get_voiced_gain_factor() const;
		void set_voiced_gain_factor(double factor);
		double get_voiced_coeff_stability_factor() const;
		void set_voiced_coeff_stability_factor(double factor);
		double get_breath() const;
		void set_breath(double breath);
		void set_interpolate_coefficients(bool interpolate);
		bool get_interpolate_coefficients() const;


	};
}

#endif // TTS_GD_LPC_SYNTHESIZER_H
