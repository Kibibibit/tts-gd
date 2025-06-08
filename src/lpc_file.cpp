#include "lpc_file.h"

#include <godot_cpp/core/class_db.hpp>
#include <fstream>
#include <string>
#include <cerrno>

using namespace godot;

static const int STEP_N_COEFFICIENTS = 0; // N coefficients read step
static const int STEP_A = 1;			  // Coefficients read step
static const int STEP_GAIN = 2;			  // Gain read step

bool LPCFile::getline(Ref<FileAccess> file, std::string &line) const
{
	// Reads a line from the file and returns true if successful, false if EOF or error
	line.clear();
	if (file.is_null())
	{
		return false; // File is not open
	}
	if (file->eof_reached())
	{
		return false; // End of file reached
	}
	String str_line = file->get_line();

	line = str_line.utf8().get_data(); // Convert Godot String to std::string
	return true;
}

std::string LPCFile::_strip_edges(const std::string &line) const
{
	// Helper function to strip leading and trailing whitespace from a string
	size_t first = line.find_first_not_of(" \t\n\r");
	size_t last = line.find_last_not_of(" \t\n\r");
	if (first == std::string::npos)
	{
		return ""; // Return empty string if there are no non-whitespace characters
	}
	std::string out = line.substr(first, (last - first + 1));
	return line.substr(first, (last - first + 1));
}

Error LPCFile::_parse_line_as_double(const std::string &line, double &value) const
{
	errno = 0;
	std::string stripped_line = _strip_edges(line.substr(line.find('=') + 1));
	char *end;
	double parsed_value = std::strtod(stripped_line.c_str(), &end);
	if (errno == ERANGE || *end != '\0')
	{
		std::string error = "Invalid double value in line: " + line;
		UtilityFunctions::push_error(String(error.c_str()), __FILE__, __LINE__);
		return ERR_INVALID_DATA;
	}
	value = parsed_value;
	return OK;
}

Error LPCFile::_parse_line_as_int(const std::string &line, int &value, int base) const
{
	errno = 0;
	std::string stripped_line = _strip_edges(line.substr(line.find('=') + 1));
	char *end;
	float parsed_value = std::strtol(stripped_line.c_str(), &end, base);
	if (errno == ERANGE || *end != '\0')
	{
		std::string error = "Invalid int value in line: " + line;
		UtilityFunctions::push_error(String(error.c_str()), ":", __FILE__, ":", __LINE__);
		return ERR_INVALID_DATA;
	}
	value = parsed_value;
	return OK;
}

void LPCFile::_step_line(Ref<FileAccess> file, std::string &line) const
{
	// Increments line, strips whitespace. If it is empty, repeats until a non-empty line is found.
	while (getline(file, line))
	{
		line = _strip_edges(line);
		if (!line.empty())
		{
			break; // Found a non-empty line
		}
	}
}

Error LPCFile::_get_double_from_file(Ref<FileAccess> file, const std::string &key, double &value)
{
	std::string line;
	_step_line(file, line);
	if (line.find(key) == std::string::npos)
	{
		std::string error = "Missing key '" + key + "' in line: " + line;
		UtilityFunctions::push_error(String(error.c_str()), __FILE__, __LINE__);
		return ERR_FILE_UNRECOGNIZED;
	}

	double out = 0.0f;
	if (_parse_line_as_double(line, out) != OK)
	{
		std::string error = "Invalid float double in line: " + line;
		UtilityFunctions::push_error(String(error.c_str()), __FILE__, __LINE__);
		return ERR_INVALID_DATA;
	}
	value = out;

	return OK;
}

Error LPCFile::_get_int_from_file(Ref<FileAccess> file, const std::string &key, int &value)
{
	std::string line;
	_step_line(file, line);
	if (line.find(key) == std::string::npos)
	{
		std::string error = "Missing key '" + key + "' in line: " + line;
		UtilityFunctions::push_error(String(error.c_str()), __FILE__, __LINE__);
		return ERR_FILE_UNRECOGNIZED;
	}

	int out = 0;
	if (_parse_line_as_int(line, out) != OK)
	{
		std::string error = "Invalid int value in line: " + line;
		UtilityFunctions::push_error(String(error.c_str()), __FILE__, __LINE__);
		return ERR_INVALID_DATA;
	}
	value = out;
	return OK;
}

LPCFile::LPCFile()
{
	x_min = 0.0f;
	x_max = 0.0f;
	frame_count = 0;
	dx = 0.0f;
	x1 = 0.0f;
	sampling_period = 1.0f / 44100.0f; // Default sample rate of 44100 Hz
	frames = nullptr;
}

LPCFile::~LPCFile()
{
	if (frames)
	{
		delete[] frames;
		frames = nullptr;
	}
}

void LPCFile::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_x_min"), &LPCFile::get_x_min);
	ClassDB::bind_method(D_METHOD("get_x_max"), &LPCFile::get_x_max);
	ClassDB::bind_method(D_METHOD("get_frame_count"), &LPCFile::get_frame_count);
	ClassDB::bind_method(D_METHOD("get_dx"), &LPCFile::get_dx);
	ClassDB::bind_method(D_METHOD("get_x1"), &LPCFile::get_x1);
	ClassDB::bind_method(D_METHOD("get_sampling_period"), &LPCFile::get_sampling_period);
	ClassDB::bind_method(D_METHOD("get_max_n_coefficients"), &LPCFile::get_max_n_coefficients);
	ClassDB::bind_method(D_METHOD("load_from_text", "file"), &LPCFile::load_from_text);
	ClassDB::bind_method(D_METHOD("load_from_binary", "file"), &LPCFile::load_from_binary);
	ClassDB::bind_method(D_METHOD("save_to_text", "file"), &LPCFile::save_to_text);
	ClassDB::bind_method(D_METHOD("save_to_binary", "file"), &LPCFile::save_to_binary);
	ClassDB::bind_method(D_METHOD("get_sample_rate"), &LPCFile::get_sample_rate);
}

void LPCFile::set_frame_count(int count)
{
	if (count < 0)
	{
		count = 0;
	}
	if (frames)
	{
		delete[] frames;
	}
	frame_count = count;
	frames = new LPCFrame[frame_count];
}

void LPCFile::set_frame(int index, const LPCFrame &frame)
{
	if (index < 0 || index >= frame_count)
	{
		UtilityFunctions::push_error("Index out of bounds in set_frame", __FILE__, __LINE__);
		return; // Out of bounds
	}
	frames[index] = frame;
}

double LPCFile::get_x_min() const
{
	return x_min;
}
double LPCFile::get_x_max() const
{
	return x_max;
}
int LPCFile::get_frame_count() const
{
	return frame_count;
}
double LPCFile::get_dx() const
{
	return dx;
}
double LPCFile::get_x1() const
{
	return x1;
}
double LPCFile::get_sampling_period() const
{
	return sampling_period;
}
int LPCFile::get_max_n_coefficients() const
{
	return max_n_coefficients;
}
LPCFile::LPCFrame *LPCFile::get_frames() const
{
	return frames;
}
double LPCFile::get_sample_rate() const
{
	// Returns the sample rate based on the sampling period
	if (sampling_period <= 0.0)
	{
		return 0.0; // Avoid division by zero
	}
	return 1.0 / sampling_period;
}

Error LPCFile::load_from_text(Ref<FileAccess> file)
{
	if (!file.is_valid())
	{
		return ERR_INVALID_PARAMETER;
	}

	if (file->get_length() == 0)
	{
		UtilityFunctions::push_error("File is empty: " + file->get_path(), __FILE__, __LINE__);
		return ERR_FILE_EOF;
	}

	if (!file->is_open())
	{
		UtilityFunctions::push_error("Failed to open file for reading: " + file->get_path(), __FILE__, __LINE__);
		return ERR_FILE_CANT_OPEN;
	}

	// First line is the file type and should read File 'type = "ooTextFile"'
	std::string line;
	if (!getline(file, line) || line.find("File type = \"ooTextFile\"") == std::string::npos)
	{
		UtilityFunctions::push_error("Invalid file format: " + file->get_path(), __FILE__, __LINE__);
		return ERR_FILE_UNRECOGNIZED;
	}
	// Next line is the object class and should read 'Object class = "LPC 1"'
	if (!getline(file, line) || line.find("Object class = \"LPC 1\"") == std::string::npos)
	{
		UtilityFunctions::push_error("Invalid object class in file: " + file->get_path(), __FILE__, __LINE__);
		return ERR_FILE_UNRECOGNIZED;
	}

	int nx = 0;

	if (_get_double_from_file(file, "xmin", x_min) != OK)
	{
		return ERR_FILE_UNRECOGNIZED;
	}
	if (_get_double_from_file(file, "xmax", x_max) != OK)
	{
		return ERR_FILE_UNRECOGNIZED;
	}
	// Frame count is listed as 'nx' in the file
	if (_get_int_from_file(file, "nx", nx) != OK)
	{
		return ERR_FILE_UNRECOGNIZED;
	}
	if (_get_double_from_file(file, "dx", dx) != OK)
	{
		return ERR_FILE_UNRECOGNIZED;
	}
	if (_get_double_from_file(file, "x1", x1) != OK)
	{
		return ERR_FILE_UNRECOGNIZED;
	}
	if (_get_double_from_file(file, "samplingPeriod", sampling_period) != OK)
	{
		return ERR_FILE_UNRECOGNIZED;
	}
	if (_get_int_from_file(file, "maxnCoefficients", max_n_coefficients) != OK)
	{
		return ERR_FILE_UNRECOGNIZED;
	}

	set_frame_count(nx);

	int assigned_frames = 0;
	int n_coefficients = 0;
	int assigned_coefficients = 0;
	int step = STEP_N_COEFFICIENTS; // Start with reading the number of coefficients
	while (assigned_frames < frame_count)
	{
		if (!getline(file, line))
		{
			UtilityFunctions::push_error("Unexpected end of file while reading frames: " + file->get_path(), __FILE__, __LINE__);
			return ERR_FILE_EOF;
		}
		line = _strip_edges(line);
		// We can skip any lines that include a colon, as they are not frame data
		if (line.find(":") != std::string::npos)
		{
			continue;
		}
		switch (step)
		{
		case STEP_N_COEFFICIENTS:
		{
			frames[assigned_frames] = LPCFrame();		// Initialize the frame
			frames[assigned_frames].n_coefficients = 0; // Reset the coefficient count for this frame
			frames[assigned_frames].gain = 0.0f;		// Reset the gain for this frame
			for (int i = 0; i < max_n_coefficients; i++)
			{
				frames[assigned_frames].coefficients[i] = 0.0f; // Initialize coefficients to zero
			}
			// Read the number of coefficients for this frame
			if (_parse_line_as_int(line, n_coefficients) != OK)
			{
				UtilityFunctions::push_error("Unparsable number of coefficients", __FILE__, __LINE__);
				return ERR_INVALID_DATA;
			}
			if (n_coefficients < 0 || n_coefficients > max_n_coefficients)
			{
				UtilityFunctions::push_error("Invalid number of coefficients", __FILE__, __LINE__);
				return ERR_INVALID_DATA;
			}
			frames[assigned_frames].n_coefficients = n_coefficients; // Set the number of coefficients for this frame
			step = STEP_A;											 // Move to reading coefficients
			assigned_coefficients = 0;								 // Reset assigned coefficients for this frame
		};
		break;
		case STEP_A:
		{
			// Read the coefficients for this frame
			double coefficient = 0.0f;
			if (_parse_line_as_double(line, coefficient) != OK)
			{
				UtilityFunctions::push_error("Unparsable coefficient value", __FILE__, __LINE__);
				return ERR_INVALID_DATA;
			}
			frames[assigned_frames].coefficients[assigned_coefficients] = coefficient;
			assigned_coefficients++;
			if (assigned_coefficients > n_coefficients)
			{
				UtilityFunctions::push_error("Too many coefficients assigned for frame", __FILE__, __LINE__);
				return ERR_INVALID_DATA;
			}
			else if (assigned_coefficients == n_coefficients)
			{
				step = STEP_GAIN; // Move to reading gain
				continue;
			}
		};
		break;
		case STEP_GAIN:
		{
			// Read the gain for this frame
			if (_parse_line_as_double(line, frames[assigned_frames].gain) != OK)
			{
				UtilityFunctions::push_error("Unparseable gain value", __FILE__, __LINE__);
				return ERR_INVALID_DATA;
			}
			assigned_frames++;			// Move to the next frame
			step = STEP_N_COEFFICIENTS; // Reset to reading number of coefficients for the next frame
		};
		break;
		}
	}

	return OK;
}

Error LPCFile::save_to_text(Ref<FileAccess> file) const
{
	if (!file.is_valid())
	{
		return ERR_INVALID_PARAMETER;
	}

	// Start by writing the header
	file->store_string("File type = \"ooTextFile\" \n");
	file->store_string("Object class = \"LPC 1\" \n");
	file->store_string("\n");
	file->store_string("xmin = " + String::num_real(x_min) + " \n");
	file->store_string("xmax = " + String::num_real(x_max) + " \n ");
	file->store_string("nx = " + String::num_int64(frame_count) + " \n");
	file->store_string("dx = " + String::num_real(dx) + " \n");
	file->store_string("x1 = " + String::num_real(x1) + " \n");
	file->store_string("samplingPeriod = " + String::num_real(sampling_period) + " \n");
	file->store_string("maxnCoefficients = " + String::num_int64(max_n_coefficients) + " \n");

	// Now for frames
	file->store_string("frames []: \n ");
	for (int i = 0; i < frame_count; i++)
	{
		const LPCFrame &frame = frames[i];
		file->store_string("    frames [" + String::num_int64(i + 1) + "]: \n");
		file->store_string("        nCoefficients = " + String::num_int64(frame.n_coefficients) + " \n");
		file->store_string("        a []: \n");
		for (int j = 0; j < frame.n_coefficients; j++)
		{
			file->store_string("            a [" + String::num_int64(j + 1) + "] = " + String::num_real(frame.coefficients[j]) + " \n");
		}
		file->store_string("		gain = " + String::num_real(frame.gain) + " \n");
	}
	file->store_string("\n");

	return OK;
}

Error LPCFile::load_from_binary(Ref<FileAccess> file)
{
	if (!file.is_valid())
	{
		return ERR_INVALID_PARAMETER;
	}

	if (file->get_length() == 0)
	{
		UtilityFunctions::push_error("File is empty: " + file->get_path(), __FILE__, __LINE__);
		return ERR_FILE_EOF;
	}

	// Check for the magic number at the start of the file
	if (file->get_pascal_string() != "LPC")
	{ // 'LPC' in ASCII
		UtilityFunctions::push_error("Invalid file format: " + file->get_path(), __FILE__, __LINE__);
		return ERR_FILE_UNRECOGNIZED;
	}
	// Read the header
	x_min = file->get_double();
	x_max = file->get_double();
	frame_count = file->get_64();
	dx = file->get_double();
	x1 = file->get_double();
	sampling_period = file->get_double();
	max_n_coefficients = file->get_64();
	set_frame_count(frame_count);
	// Read the frames
	for (int i = 0; i < frame_count; i++)
	{
		LPCFrame &frame = frames[i];
		frame.n_coefficients = file->get_64();
		if (frame.n_coefficients < 0 || frame.n_coefficients > max_n_coefficients)
		{
			UtilityFunctions::push_error("Invalid number of coefficients in frame " + String::num_int64(i), __FILE__, __LINE__);
			return ERR_INVALID_DATA;
		}
		for (int j = 0; j < frame.n_coefficients; j++)
		{
			frame.coefficients[j] = file->get_double();
		}
		frame.gain = file->get_double();
	}

	return OK;
}

Error LPCFile::save_to_binary(Ref<FileAccess> file) const
{
	if (!file.is_valid())
	{
		return ERR_INVALID_PARAMETER;
	}

	// Write the magic number
	file->store_pascal_string("LPC"); // 'LPC' in ASCII
	// Write the header
	file->store_double(x_min);
	file->store_double(x_max);
	file->store_64(frame_count);
	file->store_double(dx);
	file->store_double(x1);
	file->store_double(sampling_period);
	file->store_64(max_n_coefficients);
	// Write the frames
	for (int i = 0; i < frame_count; i++)
	{
		const LPCFrame &frame = frames[i];
		file->store_64(frame.n_coefficients);
		for (int j = 0; j < frame.n_coefficients; j++)
		{
			file->store_double(frame.coefficients[j]);
		}
		file->store_double(frame.gain);
	}

	return OK;
}