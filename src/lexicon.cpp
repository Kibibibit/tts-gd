#include "lexicon.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <unordered_map>
#include <string>
#include <set>
#include "phoneme.h"

using namespace godot;



Lexicon::Lexicon() {
	this->data = std::unordered_map<std::string, std::string>();
	this->cache = std::unordered_map<std::string, std::string>();
	this->phoneme_catalog = TypedArray<Phoneme>();
}
Lexicon::~Lexicon() {
	this->data.clear();
}


bool Lexicon::has_entry(const String &word) const {
	return this->data.find(_godot_to_std(word)) != this->data.end();
}



String Lexicon::get_entry(const String &word) const {
	auto it = this->data.find(_godot_to_std(word));
	if (it != this->data.end()) {
		return _std_to_godot(it->second);
	}
	return String();
}

String Lexicon::_translate_word(const String &word) {
	String stripped_word = word.strip_edges();

	if (this->has_entry(stripped_word)) {
		return this->get_entry(stripped_word);
	} else if (this->cache.find(_godot_to_std(stripped_word)) != this->cache.end()) {
		return _std_to_godot(this->cache[_godot_to_std(stripped_word)]);
	} else {
		String phonemes;
		String remaining_word = stripped_word;

		while (!remaining_word.is_empty()) {
			String longest_subword;
			bool longest_subword_found = false;

			for (int i = 1; i <= remaining_word.length(); i++) {
				String subword = remaining_word.substr(0, i);
				if (this->has_entry(subword) && subword.length() > longest_subword.length()) {
					longest_subword = subword;
					longest_subword_found = true;
				}
			}

			if (!longest_subword_found) {
				return String(); 
			}

			phonemes += this->get_entry(longest_subword) + " ";
			remaining_word = remaining_word.substr(longest_subword.length());
		}
		phonemes = phonemes.strip_edges();
		if (phonemes.is_empty()) {
			return String(); 
		}
		// If the word isn't in cache, add it to the cache
		if (this->cache.find(_godot_to_std(stripped_word)) == this->cache.end()) {
			this->cache[_godot_to_std(stripped_word)] = _godot_to_std(phonemes);
		}


		return phonemes.strip_edges();
	}
}


String Lexicon::translate(const String &text) {
	PackedStringArray words = normalize_text(text).split(" ");
	PackedStringArray out_phonemes = PackedStringArray();
	for (const String &word : words) {
		if (word.strip_edges().is_empty()) {
			continue;
		}
		String phonemes = _translate_word(word);
		if (!phonemes.is_empty()) {
			out_phonemes.push_back(phonemes);
		} else {
			out_phonemes.push_back(word); 
		}
	}
	return String(" ").join(out_phonemes);
}


String Lexicon::normalize_text(const String &text) const {

	// TODO: make these constants
	PackedStringArray numbers = PackedStringArray({
		"zero", "one", "two", "three", "four",
		"five", "six", "seven", "eight", "nine"
	});
	String valid_characters = "abcdefghijklmnopqrstuvwxyz' ";

	String normalized_text = text.strip_edges().to_lower();
	String result = "";

	for (int i = 0; i < normalized_text.length(); i++) {
		String current_char = normalized_text.substr(i, 1);
		if (current_char.is_valid_int()) {
			int digit = current_char.to_int();
			if (digit >= 0 && digit < 10) {
				result += numbers[digit];
			}
		} else if (valid_characters.find(current_char) != -1) {
			result += current_char;
		}
	}

	return result.strip_edges();
}

Error Lexicon::save_to_file(Ref<FileAccess> file) const {
	if (file.is_null()) {
		return ERR_INVALID_PARAMETER;
	}

	for (const auto &entry : this->data) {
		String line = _std_to_godot(entry.first) + " " + _std_to_godot(entry.second);
		file->store_line(line);
	}
	return OK;
}

Error Lexicon::load_from_file(Ref<FileAccess> file) {
	if (file.is_null()) {
		return ERR_INVALID_PARAMETER;
	}

	this->data.clear();
	this->cache.clear();
	this->phoneme_catalog.clear();
	std::set<std::string> phoneme_set;

	while (!file->eof_reached()) {
		String line = file->get_line();
		line = line.split("#")[0].strip_edges();
		if (line.begins_with(";") || line.begins_with("#") || line.find(")") != -1) {
			continue; 
		}
		PackedStringArray parts = line.strip_edges().split(" ");
		if (parts.size() < 2) {
			continue; 
		}
		String word = parts[0].to_lower();
		PackedStringArray phonemes_array = parts.slice(1, parts.size());

		for (int i = 0; i < phonemes_array.size(); ++i) {
			std::string phoneme_std_str = _godot_to_std(phonemes_array[i]);
			phoneme_set.insert(phoneme_std_str);
		}

		String phonemes = String(" ").join(phonemes_array).strip_edges();
		this->data[_godot_to_std(word)] = _godot_to_std(phonemes);
	}

	for (const std::string &phoneme_str : phoneme_set) {
		Ref<Phoneme> phoneme = Ref<Phoneme>(memnew(Phoneme));
		phoneme->set_full_name(_std_to_godot(phoneme_str));
		this->phoneme_catalog.push_back(phoneme);
	}

	return OK;
}

TypedArray<Phoneme> Lexicon::get_phoneme_catalog() const {
	return this->phoneme_catalog;
}

TypedArray<String> Lexicon::get_root_phoneme_catalog() const {
	TypedArray<Phoneme> phoneme_array = get_phoneme_catalog();
	TypedArray<String> root_phoneme_array;
	for (int i = 0; i < phoneme_array.size(); ++i) {
		Ref<Phoneme> phoneme = phoneme_array[i];
		if (phoneme.is_valid()) {
			String root_phoneme = phoneme->get_phoneme_name();
			if (!root_phoneme.is_empty() && !root_phoneme_array.has(root_phoneme)) {
				root_phoneme_array.push_back(root_phoneme);
			}
		}
	}
	return root_phoneme_array;
}

TypedArray<String> Lexicon::get_phoneme_stress_variants(Ref<Phoneme> phoneme) const {
	TypedArray<String> variants;
	TypedArray<Phoneme> all_phonemes = get_phoneme_catalog();
	for (int i = 0; i < all_phonemes.size(); ++i) {
		Ref<Phoneme> current_phoneme = all_phonemes[i];
		if (current_phoneme->get_phoneme_name() == phoneme->get_phoneme_name()) {
			String full_name = current_phoneme->get_full_name();
			if (!variants.has(full_name)) {
				variants.push_back(full_name);
			}
		}
	}
	return variants;
}
TypedArray<String> Lexicon::get_phoneme_stress_variants_from_full_name(const String &phoneme_full_name) const {
	Ref<Phoneme> phoneme = memnew(Phoneme);
	phoneme->set_full_name(phoneme_full_name);
	return get_phoneme_stress_variants(phoneme);
}
TypedArray<String> Lexicon::get_phoneme_stress_variants_from_name(const String &phoneme_name) const {
	Ref<Phoneme> phoneme = memnew(Phoneme);
	phoneme->set_phoneme_name(phoneme_name);
	return get_phoneme_stress_variants(phoneme);
}

		
void Lexicon::_bind_methods() {
	ClassDB::bind_method(D_METHOD("normalize_text", "text"), &Lexicon::normalize_text);
	ClassDB::bind_method(D_METHOD("translate", "text"), &Lexicon::translate);
	ClassDB::bind_method(D_METHOD("get_entry", "word"), &Lexicon::get_entry);
	ClassDB::bind_method(D_METHOD("has_entry", "word"), &Lexicon::has_entry);
	ClassDB::bind_method(D_METHOD("save_to_file", "file"), &Lexicon::save_to_file);
	ClassDB::bind_method(D_METHOD("load_from_file", "file"), &Lexicon::load_from_file);
	ClassDB::bind_method(D_METHOD("get_phoneme_catalog"), &Lexicon::get_phoneme_catalog);
	ClassDB::bind_method(D_METHOD("get_root_phoneme_catalog"), &Lexicon::get_root_phoneme_catalog);
	ClassDB::bind_method(D_METHOD("get_phoneme_stress_variants", "phoneme"), &Lexicon::get_phoneme_stress_variants);
	ClassDB::bind_method(D_METHOD("get_phoneme_stress_variants_from_full_name", "phoneme_full_name"), &Lexicon::get_phoneme_stress_variants_from_full_name);
	ClassDB::bind_method(D_METHOD("get_phoneme_stress_variants_from_name", "phoneme_name"), &Lexicon::get_phoneme_stress_variants_from_name);


}
