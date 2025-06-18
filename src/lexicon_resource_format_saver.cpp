#include "lexicon_resource_format_saver.h"
#include "lexicon.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void LexiconResourceFormatSaver::_bind_methods()
{
	// methods are already bound in ResourceFormatSaver
}

Error LexiconResourceFormatSaver::_save(const Ref<Resource> &resource, const String &path, uint32_t flags)
{
	if (resource.is_null() || !resource->is_class("Lexicon"))
	{
		UtilityFunctions::push_error("Resource is not a valid Lexicon", __FILE__, __LINE__);
		return ERR_INVALID_DATA;
	}

	Ref<Lexicon> lexicon = resource;
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::WRITE);
	if (file.is_null())
	{
		UtilityFunctions::push_error("Failed to open file for writing: " + path, __FILE__, __LINE__);
		return ERR_CANT_OPEN;
	}

	String extension = path.get_extension().to_lower();
	
	Error err = lexicon->save_to_file(file);

	file->close();
	return err;
}

PackedStringArray LexiconResourceFormatSaver::_get_recognized_extensions(const Ref<Resource> &resource) const
{
	if (resource.is_valid() && resource->is_class("Lexicon"))
	{
		PackedStringArray extensions;
		extensions.push_back("dict");
		return extensions;
	}
	return PackedStringArray();
}

bool LexiconResourceFormatSaver::_recognize(const Ref<Resource> &resource) const
{
	return resource.is_valid() && resource->is_class("Lexicon");
}