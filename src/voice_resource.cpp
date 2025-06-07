#include "voice_resource.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

void VoiceResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_voice_name", "name"), &VoiceResource::set_voice_name);
	ClassDB::bind_method(D_METHOD("get_voice_name"), &VoiceResource::get_voice_name);

	ClassDB::bind_method(D_METHOD("set_voice_language", "language"), &VoiceResource::set_voice_language);
	ClassDB::bind_method(D_METHOD("get_voice_language"), &VoiceResource::get_voice_language);

	ClassDB::bind_method(D_METHOD("set_lpc_file", "file"), &VoiceResource::set_lpc_file);
	ClassDB::bind_method(D_METHOD("get_lpc_file"), &VoiceResource::get_lpc_file);

	ClassDB::bind_method(D_METHOD("set_phones_map", "phones"), &VoiceResource::set_phones_map);
	ClassDB::bind_method(D_METHOD("get_phones_map"), &VoiceResource::get_phones_map);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "voice_name"), "set_voice_name", "get_voice_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "voice_language"), "set_voice_language", "get_voice_language");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lpc_file", PROPERTY_HINT_RESOURCE_TYPE, "LPCFile"), "set_lpc_file", "get_lpc_file");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "phone_map"), "set_phones_map", "get_phones_map");

}
VoiceResource::VoiceResource() {
	voice_name = "";
	voice_language = "";
	lpc_file = Ref<LPCFile>();
	phone_map = TypedDictionary<String, Phone>();
}
VoiceResource::~VoiceResource() {
	// Destructor logic if needed
}

void VoiceResource::set_voice_name(const String &name) {
	voice_name = name;
}
String VoiceResource::get_voice_name() const {
	return voice_name;
}
void VoiceResource::set_voice_language(const String &language) {
	voice_language = language;
}
String VoiceResource::get_voice_language() const {
	return voice_language;
}
void VoiceResource::set_lpc_file(const Ref<LPCFile> &file) {
	lpc_file = file;
}
Ref<LPCFile> VoiceResource::get_lpc_file() const {
	return lpc_file;
}
void VoiceResource::set_phones_map(const TypedDictionary<String, Phone> &phones) {
	phone_map = phones;
}
TypedDictionary<String, Phone> VoiceResource::get_phones_map() const {
	return phone_map;
}