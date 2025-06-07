#include "voice_resource.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void VoiceResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_voice_name", "name"), &VoiceResource::set_voice_name);
	ClassDB::bind_method(D_METHOD("get_voice_name"), &VoiceResource::get_voice_name);

	ClassDB::bind_method(D_METHOD("set_voice_language", "language"), &VoiceResource::set_voice_language);
	ClassDB::bind_method(D_METHOD("get_voice_language"), &VoiceResource::get_voice_language);

	ClassDB::bind_method(D_METHOD("set_lpc_file", "file"), &VoiceResource::set_lpc_file);
	ClassDB::bind_method(D_METHOD("get_lpc_file"), &VoiceResource::get_lpc_file);

	ClassDB::bind_method(D_METHOD("set_phoneme_map", "map"), &VoiceResource::set_phoneme_map);
	ClassDB::bind_method(D_METHOD("get_phoneme_map"), &VoiceResource::get_phoneme_map);
}
VoiceResource::VoiceResource() {
	voice_name = "";
	voice_language = "";
	lpc_file = Ref<LPCFile>();
	phoneme_map = TypedDictionary<String, int>();
}
VoiceResource::~VoiceResource() {
	// Destructor logic if needed
}