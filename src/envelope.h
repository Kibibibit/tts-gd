#ifndef TTS_GD_ENVELOPE_H
#define TTS_GD_ENVELOPE_H

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

	class Envelope: public RefCounted {
		GDCLASS(Envelope, RefCounted);
		private:
			double attack_time;
			double decay_time;
			double sustain_time;
			double sustain_level;
			double release_time;

		protected:
			static void _bind_methods();

		public:
			Envelope();
			~Envelope();

			double get_total_time() const;

			double amplitude_at(double time) const;

			void set_attack_time(double attack_time);
			double get_attack_time() const;

			void set_decay_time(double decay_time);
			double get_decay_time() const;

			void set_sustain_level(double sustain_level);
			double get_sustain_level() const;

			void set_sustain_time(double sustain_time);
			double get_sustain_time() const;

			void set_release_time(double release_time);
			double get_release_time() const;
	};
}

#endif // TTS_GD_ENVELOPE_H

