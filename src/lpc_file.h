#ifndef TTS_GD_LPC_FILE_H
#define TTS_GD_LPC_FILE_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <string>
#include <fstream>

namespace godot {

class LPCFile : public Resource {
	GDCLASS(LPCFile, Resource);

	public:
		struct LPCFrame {
			double coefficients[16];
			double gain;
			int n_coefficients;
		};
	
	private:
		// Starting point of the recording, in seconds. Usually 0.0
		double x_min;
		// Ending point of the recording, in seconds
		double x_max;
		// Number of frames
		int frame_count;
		// Time in seconds between frames
		double dx;
		// Starting point of the first frame, in seconds
		double x1;
		// 1 over the sample rate, in seconds
		double sampling_period;
		int max_n_coefficients = 16;

		LPCFrame *frames = nullptr;

		bool getline(Ref<FileAccess> file, std::string &line) const;
		std::string _strip_edges(const std::string &line) const;
		void _step_line(Ref<FileAccess> file, std::string &line) const;
		Error _parse_line_as_double(const std::string &line, double &value) const;
		Error _parse_line_as_int(const std::string &line, int &value, int base = 10) const;
		Error _get_int_from_file(Ref<FileAccess> file, const std::string &key, int &value);
		Error _get_double_from_file(Ref<FileAccess> file, const std::string &key, double &value);
	

	protected:
		static void _bind_methods();
	
	public:
		LPCFile();
		~LPCFile();

		LPCFrame *get_frames() const;
		void set_frame_count(int count);
		void set_frame(int index, const LPCFrame &frame);

		// Loaders
		Error load_from_text(Ref<FileAccess> file);
		Error load_from_binary(Ref<FileAccess> file);

		// Savers
		Error save_to_text(Ref<FileAccess> file) const;
		Error save_to_binary(Ref<FileAccess> file) const;

		// Exposed methods
		double get_x_min() const;
		double get_x_max() const;
		int get_frame_count() const;
		double get_dx() const;
		double get_x1() const;
		double get_sampling_period() const;
		int get_max_n_coefficients() const;

		double get_sample_rate() const;


	};
}









#endif // TTS_GD_LPC_FILE_H
