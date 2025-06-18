#ifndef TTS_GD_LEXICON_RESOURCE_FORMAT_LOADER_H
#define TTS_GD_LEXICON_RESOURCE_FORMAT_LOADER_H

#include <godot_cpp/classes/resource_format_loader.hpp>


namespace godot {
	class LexiconResourceFormatLoader : public ResourceFormatLoader {
		GDCLASS(LexiconResourceFormatLoader, ResourceFormatLoader);

		protected:
			static void _bind_methods();

		public:
			LexiconResourceFormatLoader();
			~LexiconResourceFormatLoader();
			Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;

			PackedStringArray _get_recognized_extensions() const override;
        	bool _handles_type(const StringName &type) const override;
        	String _get_resource_type(const String &path) const override;
		
	};
			
}






#endif // TTS_GD_LPC_RESOURCE_FORMAT_LOADER_H

