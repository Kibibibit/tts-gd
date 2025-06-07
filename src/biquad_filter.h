#ifndef TTS_GD_BIQUAD_FILTER_H
#define TTS_GD_BIQUAD_FILTER_H

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot
{
	class BiquadFilter : public RefCounted
	{
		GDCLASS(BiquadFilter, RefCounted);	

		private:
			bool is_notch;
			double frequency;
			double q;
			double gain; 
			// State
			double x1, x2, y1, y2;
			// Coefficients
			double b0, b1, b2, a1, a2;

			double sample_rate;

			void _update_coefficients();

		protected:
			static void _bind_methods();

		public:
			BiquadFilter();
			~BiquadFilter();

			void reset_state();

			void initialize(double sample_rate, bool notch, double frequency, double q, double gain);

			void set_sample_rate(double sample_rate);
			double get_sample_rate() const;

			void set_frequency(double frequency);
			double get_frequency() const;
			void set_q(double q);
			double get_q() const;
			void set_gain(double gain);
			double get_gain() const;


			double process_signal(double input);
		
			void set_a1(double a1);
			double get_a1() const;
			void set_a2(double a2);
			double get_a2() const;
			void set_b0(double b0);
			double get_b0() const;
			void set_b1(double b1);
			double get_b1() const;
			void set_b2(double b2);
			double get_b2() const;

			bool is_notch_filter() const;
			void set_notch_filter(bool is_notch);
	};
}




#endif // TTS_GD_BIQUAD_FILTER_H

