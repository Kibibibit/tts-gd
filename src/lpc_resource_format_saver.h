#ifndef TTS_GD_LPC_RESOURCE_FORMAT_SAVER_H
#define TTS_GD_LPC_RESOURCE_FORMAT_SAVER_H

#include <godot_cpp/classes/resource_format_saver.hpp>
#include <godot_cpp/classes/resource.hpp>


namespace godot {

class LPCResourceFormatSaver : public ResourceFormatSaver {
    GDCLASS(LPCResourceFormatSaver, ResourceFormatSaver)

protected:
    static void _bind_methods();

public:
    Error _save(const Ref<Resource> &resource, const String &path, uint32_t flags = 0) override;
    PackedStringArray _get_recognized_extensions(const Ref<Resource> &resource) const override;
    bool _recognize(const Ref<Resource> &resource) const override;
};

}

#endif // TTS_GD_LPC_RESOURCE_FORMAT_SAVER_H