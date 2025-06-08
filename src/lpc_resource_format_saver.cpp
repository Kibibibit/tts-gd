#include "lpc_resource_format_saver.h"
#include "lpc_file.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void LPCResourceFormatSaver::_bind_methods()
{
	// methods are already bound in ResourceFormatSaver
}

Error LPCResourceFormatSaver::_save(const Ref<Resource> &resource, const String &path, uint32_t flags)
{
	if (resource.is_null() || !resource->is_class("LPCFile"))
	{
		UtilityFunctions::push_error("Resource is not a valid LPCFile", __FILE__, __LINE__);
		return ERR_INVALID_DATA;
	}

	Ref<LPCFile> lpc_file = resource;
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::WRITE);
	if (file.is_null())
	{
		UtilityFunctions::push_error("Failed to open file for writing: " + path, __FILE__, __LINE__);
		return ERR_CANT_OPEN;
	}

	String extension = path.get_extension().to_lower();
	Error err;
	if (extension == "lpc")
	{
		err = lpc_file->save_to_text(file);
		return err;
	}
	else if (extension == "lpcb")
	{
		err = lpc_file->save_to_binary(file);
	}
	else
	{
		UtilityFunctions::push_error("Unsupported file extension: " + extension, __FILE__, __LINE__);
		err = ERR_FILE_UNRECOGNIZED;
	}

	file->close();
	return err;
}

PackedStringArray LPCResourceFormatSaver::_get_recognized_extensions(const Ref<Resource> &resource) const
{
	if (resource.is_valid() && resource->is_class("LPCFile"))
	{
		PackedStringArray extensions;
		extensions.push_back("lpc");
		extensions.push_back("LPC");
		extensions.push_back("lpcb");
		extensions.push_back("LPCB");
		return extensions;
	}
	return PackedStringArray();
}

bool LPCResourceFormatSaver::_recognize(const Ref<Resource> &resource) const
{
	return resource.is_valid() && resource->is_class("LPCFile");
}