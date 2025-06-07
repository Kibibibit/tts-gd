#ifndef TTS_GD_VOICE_RESOURCE_H
#define TTS_GD_VOICE_RESOURCE_H

#include "lpc_file.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>

namespace godot {
class VoiceResource : public Resource {
	GDCLASS(VoiceResource, Resource);

	private:
		String voice_name;
		String voice_language;
		Ref<LPCFile> lpc_file;
		TypedDictionary<String, int> phoneme_map;

	protected:
		static void _bind_methods();
	public:
		VoiceResource();
		~VoiceResource();

		// Add properties and methods specific to the voice resource here
		void set_voice_name(const String &name);
		String get_voice_name() const;

		void set_voice_language(const String &language);
		String get_voice_language() const;

		void set_lpc_file(const Ref<LPCFile> &file);
		Ref<LPCFile> get_lpc_file() const;

		void set_phoneme_map(const Dictionary &map);
		Dictionary get_phoneme_map() const;


	};

}



#endif // TTS_GD_VOICE_RESOURCE_H
