#ifndef TTS_GD_PHONEME_H
#define TTS_GD_PHONEME_H
#include <godot_cpp/classes/ref_counted.hpp>
#include "formant_set.h"
#include "envelope.h"


namespace godot
{
	class Phoneme : public RefCounted
	{
		GDCLASS(Phoneme, RefCounted);
	private:
		String symbol;
		String ipa_symbol;
		String description;

		double voice_mix_ratio;
		double f0_modifier;

		Ref<FormantSet> formant_set;
		Ref<FormantSet> end_formant_set;
		Ref<Envelope> envelope;

		bool is_diphthong;
		bool is_silence;
		
		double transition_start;
		double transition_length;
	
	protected:
		static void _bind_methods();
	public:
		Phoneme();
		~Phoneme();

		double get_typical_duration() const;

		void set_symbol(const String &symbol);
		String get_symbol() const;

		void set_ipa_symbol(const String &ipa_symbol);
		String get_ipa_symbol() const;

		void set_description(const String &description);
		String get_description() const;

		void set_voice_mix_ratio(double ratio);
		double get_voice_mix_ratio() const;

		void set_formant_set(const Ref<FormantSet> &formant_set);
		Ref<FormantSet> get_formant_set() const;

		void set_end_formant_set(const Ref<FormantSet> &end_formant_set);
		Ref<FormantSet> get_end_formant_set() const;

		Ref<FormantSet> get_effective_formant_set(double t) const;

		void set_envelope(const Ref<Envelope> &envelope);
		Ref<Envelope> get_envelope() const;

		void set_f0_modifier(double modifier);
		double get_f0_modifier() const;

		void set_is_diphthong(bool is_diphthong);
		bool get_is_diphthong() const;
		void set_is_silence(bool is_silence);
		bool get_is_silence() const;
		void set_transition_start(double start);
		double get_transition_start() const;
		void set_transition_length(double length);
		double get_transition_length() const;
		
	};
	
} // namespace godot

#endif // TTS_GD_PHONEME_H

