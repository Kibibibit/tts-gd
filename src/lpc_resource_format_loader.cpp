#include "lpc_resource_format_loader.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
LPCResourceFormatLoader::LPCResourceFormatLoader()
{
}
LPCResourceFormatLoader::~LPCResourceFormatLoader()
{
}

Variant LPCResourceFormatLoader::_load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const
{

	String extension = path.get_extension().to_lower();

	if (extension == "lpc")
	{
		return _load_from_text_file(path, original_path, use_sub_threads, cache_mode);
	}
	else if (extension == "lpcb")
	{
		return _load_from_binary_file(path, original_path, use_sub_threads, cache_mode);
	}
	else
	{
		UtilityFunctions::push_error("Unsupported file extension: " + extension, __FILE__, __LINE__);
		return Variant();
	}
}

Variant LPCResourceFormatLoader::_load_from_text_file(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const
{
	// Load the LPC file from a text format
	Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
	if (f.is_null())
	{
		UtilityFunctions::push_error("Failed to open file: " + path, __FILE__, __LINE__);
		return Variant();
	}

	LPCFile *lpc_file = memnew(LPCFile());
	Error err = lpc_file->load_from_text(f);
	f->close();

	if (err != OK)
	{
		UtilityFunctions::push_error("Error loading LPC file: " + path, __FILE__, __LINE__);
		memdelete(lpc_file);
		return Variant();
	}

	return lpc_file;
}

Variant LPCResourceFormatLoader::_load_from_binary_file(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const
{
	// Load the LPC file from a binary format
	Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
	if (f.is_null())
	{
		UtilityFunctions::push_error("Failed to open file: " + path, __FILE__, __LINE__);
		return Variant();
	}

	LPCFile *lpc_file = memnew(LPCFile());
	Error err = lpc_file->load_from_binary(f);
	f->close();

	if (err != OK)
	{
		UtilityFunctions::push_error("Error loading LPC file: " + path, __FILE__, __LINE__);
		memdelete(lpc_file);
		return Variant();
	}

	return lpc_file;
}

PackedStringArray LPCResourceFormatLoader::_get_recognized_extensions() const
{
	PackedStringArray extensions;
	extensions.push_back("lpc");
	extensions.push_back("LPC");
	extensions.push_back("lpcb");
	extensions.push_back("LPCB");
	return extensions;
}

bool LPCResourceFormatLoader::_handles_type(const StringName &type) const
{
	return type == StringName("LPCFile");
}
String LPCResourceFormatLoader::_get_resource_type(const String &path) const
{
	String extension = path.get_extension().to_lower();
	if (extension == "lpc" || extension == "lpcb")
	{
		return "LPCFile";
	}
	return "";
}

void LPCResourceFormatLoader::_bind_methods()
{
	// Methods already bound
}