#ifndef TTS_GD_LPC_SYNTHESIZER_H
#define TTS_GD_LPC_SYNTHESIZER_H

#include "lpc_file.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/typed_array.hpp>


namespace godot {
class LPCSynthesizer : public RefCounted {
	GDCLASS(LPCSynthesizer, RefCounted);

	private:
		PackedFloat32Array history;
		Ref<LPCFile> lpc_file;
		int max_order = 16; // Default LPC order
	
	protected:
		static void _bind_methods();
	public:
		LPCSynthesizer();
		~LPCSynthesizer();

		void reset();
		PackedFloat32Array get_history() const;

		void set_lpc_file(const Ref<LPCFile> &file);
		Ref<LPCFile> get_lpc_file() const;

		void set_max_order(int order);
		int get_max_order() const;

		double process_sample(double excitation, int frame_index);
		double process_sample_interpolated(double excitation, int frame_index, int next_frame_index, double frame_progress);
	};
}

#endif // TTS_GD_LPC_SYNTHESIZER_H
