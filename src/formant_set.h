#ifndef TTS_GD_FORMANT_SET_H
#define TTS_GD_FORMANT_SET_H

#include <godot_cpp/classes/ref_counted.hpp>


namespace godot {
	
	class FormantSet : public RefCounted {
		GDCLASS(FormantSet, RefCounted);

	private:
		PackedFloat32Array frequencies;
		PackedFloat32Array bandwidths;
		PackedFloat32Array gains;

		PackedFloat32Array antiformant_frequencies;
		PackedFloat32Array antiformant_bandwidths;


	protected:
		static void _bind_methods();

	public:
		FormantSet();
		~FormantSet();

		void set_formant_data(const PackedFloat32Array &frequencies, const PackedFloat32Array &bandwidths, const PackedFloat32Array &gains);
		void set_antiformant_data(const PackedFloat32Array &frequencies, const PackedFloat32Array &bandwidths);

		void set_formant(int index, double frequency, double bandwidth, double gain);
		double get_formant_frequency(int index) const;
		double get_formant_bandwidth(int index) const;
		double get_formant_gain(int index) const;
		void set_formant_frequency(int index, double frequency);
		void set_formant_bandwidth(int index, double bandwidth);
		void set_formant_gain(int index, double gain);
		int get_formant_count() const;
		void set_antiformant(int index, double frequency, double bandwidth);
		double get_antiformant_frequency(int index) const;
		double get_antiformant_bandwidth(int index) const;
		void set_antiformant_frequency(int index, double frequency);
		void set_antiformant_bandwidth(int index, double bandwidth);
		int get_antiformant_count() const;


		PackedFloat32Array get_frequencies() const;
		PackedFloat32Array get_bandwidths() const;
		PackedFloat32Array get_gains() const;
		PackedFloat32Array get_antiformant_frequencies() const;
		PackedFloat32Array get_antiformant_bandwidths() const;

		Ref<FormantSet> blend(const Ref<FormantSet> &other, double t) const;
	};
}


#endif // TTS_GD_FORMANT_SET_H
