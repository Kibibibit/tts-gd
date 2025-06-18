#include "register_types.h"
#include "wave_generator.h"

#include "lpc_file.h"
#include "lpc_resource_format_loader.h"
#include "lpc_resource_format_saver.h"
#include "lexicon.h"
#include "lexicon_resource_format_loader.h"
#include "lexicon_resource_format_saver.h"
#include "speech_synthesizer_node.h"
#include "phone.h"
#include "phoneme.h"
#include "voice_resource.h"
#include "lpc_synthesizer.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

using namespace godot;

static Ref<LPCResourceFormatLoader> lpc_loader;
static Ref<LPCResourceFormatSaver> lpc_saver;
static Ref<LexiconResourceFormatLoader> lexicon_loader;
static Ref<LexiconResourceFormatSaver> lexicon_saver;

void initialize_tts_gd(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }

    ClassDB::register_class<WaveGenerator>();
    ClassDB::register_class<LPCFile>();
    ClassDB::register_class<LPCResourceFormatLoader>();
    ClassDB::register_class<LPCResourceFormatSaver>();
    ClassDB::register_class<LPCSynthesizer>();
    ClassDB::register_class<Phone>();
    ClassDB::register_class<Phoneme>();
    ClassDB::register_class<VoiceResource>();
    ClassDB::register_class<Lexicon>();
    ClassDB::register_class<LexiconResourceFormatLoader>();
    ClassDB::register_class<LexiconResourceFormatSaver>();
    ClassDB::register_class<SpeechSynthesizerNode>();

    lpc_loader.instantiate();
    lpc_saver.instantiate();
    lexicon_loader.instantiate();
    lexicon_saver.instantiate();
    ResourceLoader::get_singleton()->add_resource_format_loader(lpc_loader);
    ResourceSaver::get_singleton()->add_resource_format_saver(lpc_saver);
    ResourceLoader::get_singleton()->add_resource_format_loader(lexicon_loader);
    ResourceSaver::get_singleton()->add_resource_format_saver(lexicon_saver);
}
void uninitialize_tts_gd(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
    ResourceLoader::get_singleton()->remove_resource_format_loader(lpc_loader);
    ResourceSaver::get_singleton()->remove_resource_format_saver(lpc_saver);
    ResourceLoader::get_singleton()->remove_resource_format_loader(lexicon_loader);
    ResourceSaver::get_singleton()->remove_resource_format_saver(lexicon_saver);
    lpc_loader.unref();
    lpc_saver.unref();
    lexicon_loader.unref();
    lexicon_saver.unref();
}
extern "C"
{
    GDExtensionBool GDE_EXPORT tts_gd_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_tts_gd);
        init_obj.register_terminator(uninitialize_tts_gd);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}