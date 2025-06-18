#ifndef TTS_GD_LEXICON_H
#define TTS_GD_LEXICON_H

#include <unordered_map>
#include <string>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include "phoneme.h"
#include <godot_cpp/classes/file_access.hpp>

namespace godot {
	

	class Lexicon : public Resource {
		GDCLASS(Lexicon, Resource);

	private:
		std::unordered_map<std::string, std::string> data;
		// A cache to store words that were not in data
		std::unordered_map<std::string, std::string> cache;

		TypedArray<Phoneme> phoneme_catalog;

		String _translate_word(const String &word);

		_ALWAYS_INLINE_ String _std_to_godot(const std::string &str) const {
			return String(str.c_str());
		}
		_ALWAYS_INLINE_ std::string _godot_to_std(const String &str) const {
			return std::string(str.utf8().get_data());
		}

	protected:
		static void _bind_methods();

	public:
		Lexicon();
		~Lexicon();

		String normalize_text(const String &text) const;
		String translate(const String &text);

		String get_entry(const String &word) const;
		bool has_entry(const String &word) const;

		Error save_to_file(Ref<FileAccess> file) const;
		Error load_from_file(Ref<FileAccess> file);


		// TODO: Refactor this whole system at some point
		TypedArray<Phoneme> get_phoneme_catalog() const;
		TypedArray<String> get_root_phoneme_catalog() const;
		TypedArray<String> get_phoneme_stress_variants(Ref<Phoneme> phoneme) const;
		TypedArray<String> get_phoneme_stress_variants_from_full_name(const String &phoneme_full_name) const;
		TypedArray<String> get_phoneme_stress_variants_from_name(const String &phoneme_name) const;
		
	};

}

#endif //TTS_GD_LEXICON_H