#ifndef TTS_GD_PHONE_H
#define TTS_GD_PHONE_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/typed_array.hpp>


namespace godot {
class Phone : public RefCounted {
	GDCLASS(Phone, RefCounted);
	private:
		String name; // Name of the phone
		String phoneme_ipa; // IPA representation of the phone
		String phoneme_variant; // Variant of the phoneme, if applicable
		TypedArray<int> start_lpc_indices;
		TypedArray<int> end_lpc_indices;
		TypedArray<double> transition_curve;
		double attack_time;
		double release_time;

		bool is_diphthong;

		double stressed_duration_factor;
		double stressed_amplitude_factor;
		double stressed_pitch_factor;

	protected:
		static void _bind_methods();

	public:
		Phone();
		~Phone();

		void set_name(const String &p_name);
		String get_name() const;

		void set_phoneme_ipa(const String &p_ipa);
		String get_phoneme_ipa() const;

		void set_phoneme_variant(const String &p_variant);
		String get_phoneme_variant() const;

		void set_is_diphthong(bool p_is_diphthong);
		bool get_is_diphthong() const;

		void set_start_lpc_indices(const TypedArray<int> &p_indices);
		TypedArray<int> get_start_lpc_indices() const;

		void set_end_lpc_indices(const TypedArray<int> &p_indices);
		TypedArray<int> get_end_lpc_indices() const;

		void set_transition_curve(const TypedArray<double> &p_curve);
		TypedArray<double> get_transition_curve() const;

		void set_attack_time(double p_time);
		double get_attack_time() const;

		void set_release_time(double p_time);
		double get_release_time() const;

		void set_stressed_duration_factor(double p_factor);
		double get_stressed_duration_factor() const;

		void set_stressed_amplitude_factor(float p_factor);
		double get_stressed_amplitude_factor() const;

		void set_stressed_pitch_factor(double p_factor);
		double get_stressed_pitch_factor() const;
	};
}


#endif // TTS_GD_PHONE_H
