#ifndef TTS_GD_FORMANT_FILTER_H
#define TTS_GD_FORMANT_FILTER_H


#include <godot_cpp/classes/ref_counted.hpp>


namespace godot {

	class FormantFilter : public RefCounted {
		GDCLASS(FormantFilter, RefCounted);

	public:
		enum FilterType {
			BANDPASS,
			NOTCH,
		};

		enum FilterStages {
			STAGE_1,
			STAGE_2,
			STAGE_3,
			STAGE_4,
		};

		class Processor {
			public:
				struct Coeffs {
					double a1 = 0.0;
					double a2 = 0.0;
					double b0 = 0.0;
					double b1 = 0.0;
					double b2 = 0.0;
				};
	
				Processor();
			private:
				float ha1 = 0.0f;
				float ha2 = 0.0f;
				float hb1 = 0.0f;
				float hb2 = 0.0f;
				FilterType type = BANDPASS;
				Coeffs coeffs;
				Coeffs incr_coeffs;
				FormantFilter *filter = nullptr;
			
			public:
				void set_filter(FormantFilter *p_filter, bool p_clear_history = true);
				void process(float *p_samples, int p_amount, int p_stride = 1, bool p_interpolate = false);
				void update_coeffs(int p_interp_buffer_len = 0);
				_ALWAYS_INLINE_ void process_one(float &p_sample);
				_ALWAYS_INLINE_ void process_one_interp(float &p_sample);
				void set_type(FilterType p_type);
		};

	private:
		FilterType type = BANDPASS;
		FilterStages stages = STAGE_1;
		float cutoff = 5000.0f; // Default cutoff frequency in Hz
		float resonance = 0.5f; // Default resonance factor
		float filter_gain = 1.0f; // Default filter gain (linear scale)
		float amplification_gain = 0.0f; // extra gain applied to the output
		float sampling_rate = 44100.0f; // Default sampling rate in Hz

		Processor processors[4];

		template<int S>
		void _apply_processors(float *p_src_samples,float *p_dst_samples, int p_amount);
		template<int S>
		void _apply_processors_interpolated(float *p_src_samples, float *p_dst_samples, int p_amount);


	protected:
		static void _bind_methods();
	
	public:
		FormantFilter();

		void set_type(FilterType p_type);
		void set_stages(FilterStages p_stages);
		void set_cutoff(float p_cutoff);
		void set_resonance(float p_resonance);
		void set_amplification_gain(float p_gain);
		void set_sampling_rate(float p_sampling_rate);
		void set_filter_gain(float p_gain);

		float get_cutoff() const;
		float get_resonance() const;
		float get_amplification_gain() const;
		float get_sampling_rate() const;
		float get_filter_gain() const;
		FilterType get_type() const;
		FilterStages get_stages() const;

		void reset();

		void prepare_coefficients(Processor::Coeffs *p_coeffs) const;

		void process(float *p_samples, int p_amount);
		void process_interpolated(float *p_samples, int p_amount, int p_interp_buffer_len);

		void process_buffer(PackedFloat32Array p_samples);
		void process_buffer_interpolated(PackedFloat32Array p_samples, int p_interp_buffer_len = 0);

	};

	void FormantFilter::Processor::process_one(float &p_sample) {
		float pre = p_sample;
		p_sample = (p_sample * coeffs.b0 + hb1 * coeffs.b1 + hb2 * coeffs.b2 + ha1 * coeffs.a1 + ha2 * coeffs.a2);
		ha2 = ha1;
		hb2 = hb1;
		hb1 = pre;
		ha1 = p_sample;
	}

	void FormantFilter::Processor::process_one_interp(float &p_sample) {
		float pre = p_sample;
		p_sample = (p_sample * coeffs.b0 + hb1 * coeffs.b1 + hb2 * coeffs.b2 + ha1 * coeffs.a1 + ha2 * coeffs.a2);
		ha2 = ha1;
		hb2 = hb1;
		hb1 = pre;
		ha1 = p_sample;
	
		coeffs.b0 += incr_coeffs.b0;
		coeffs.b1 += incr_coeffs.b1;
		coeffs.b2 += incr_coeffs.b2;
		coeffs.a1 += incr_coeffs.a1;
		coeffs.a2 += incr_coeffs.a2;
	}

}

VARIANT_ENUM_CAST(godot::FormantFilter::FilterType);
VARIANT_ENUM_CAST(godot::FormantFilter::FilterStages);








#endif // TTS_GD_FORMANT_FILTER_H
