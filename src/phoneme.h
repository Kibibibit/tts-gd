#ifndef TTS_GD_PHONEME_H
#define TTS_GD_PHONEME_H

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class Phoneme : public RefCounted {
	GDCLASS(Phoneme, RefCounted);
	private:
		String full_name;
		int stress = 0;
		String phoneme_name;
		bool is_vowel;

		void _parse_full_name();
		bool _validate() const;

	protected:
		static void _bind_methods();
	
	public:
		Phoneme();
		~Phoneme();

		void set_full_name(const String &name);
		String get_full_name() const;

		void set_stress(int stress);
		int get_stress() const;

		void set_phoneme_name(const String &name);
		String get_phoneme_name() const;

		void set_is_vowel(bool is_vowel);
		bool get_is_vowel() const;

		String _to_string() const;
		int _hash() const;

		
};



}

#endif // TTS_GD_PHONEME_H