#include "lexicon_resource_format_loader.h"
#include "lexicon.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
LexiconResourceFormatLoader::LexiconResourceFormatLoader()
{
}
LexiconResourceFormatLoader::~LexiconResourceFormatLoader()
{
}

Variant LexiconResourceFormatLoader::_load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const
{
	
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	if (file.is_null())
	{
		UtilityFunctions::push_error("Failed to open file: " + path, __FILE__, __LINE__);
		return Variant();
	}

	Lexicon *lexicon = memnew(Lexicon());


	Error err = lexicon->load_from_file(file);
	if (err != OK)
	{
		UtilityFunctions::push_error("Error loading Lexicon file: " + path, __FILE__, __LINE__);
		memdelete(lexicon);
		return Variant();
	}
	file->close();

	return lexicon;
}



PackedStringArray LexiconResourceFormatLoader::_get_recognized_extensions() const
{
	PackedStringArray extensions;
	extensions.push_back("dict");
	return extensions;
}

bool LexiconResourceFormatLoader::_handles_type(const StringName &type) const
{
	return type == StringName("Lexicon");
}
String LexiconResourceFormatLoader::_get_resource_type(const String &path) const
{
	String extension = path.get_extension().to_lower();
	if (extension == "dict")
	{
		return "Lexicon";
	}
	return "";
}

void LexiconResourceFormatLoader::_bind_methods()
{
	// Methods already bound
}