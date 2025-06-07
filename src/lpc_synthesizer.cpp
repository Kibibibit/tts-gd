#include "lpc_synthesizer.h"
#include "lpc_file.h"


using namespace godot;

LPCSynthesizer::LPCSynthesizer() {
	history = PackedFloat32Array();
	history.resize(max_order);
	reset();
}
LPCSynthesizer::~LPCSynthesizer() {
	// Destructor logic if needed
}

void LPCSynthesizer::reset() {
	history.fill(0.0f);
}
PackedFloat32Array LPCSynthesizer::get_history() const {
	return history;
}
void LPCSynthesizer::set_lpc_file(const Ref<LPCFile> &file) {
	lpc_file = file;
}
Ref<LPCFile> LPCSynthesizer::get_lpc_file() const {
	return lpc_file;
}
void LPCSynthesizer::set_max_order(int order) {
	max_order = order;
	history.resize(max_order);
}
int LPCSynthesizer::get_max_order() const {
	return max_order;
}
double LPCSynthesizer::process_sample(double excitation, int frame_index) {
	return process_sample_interpolated(excitation, frame_index, frame_index, 0.0);
}

double LPCSynthesizer::process_sample_interpolated(double excitation, int frame_index, int next_frame_index, double frame_progress) {
	if (!lpc_file.is_valid() || frame_index < 0 || next_frame_index < 0 || frame_index >= lpc_file->get_frame_count() || next_frame_index >= lpc_file->get_frame_count()) {
		UtilityFunctions::push_error("Invalid LPC file or frame index in process_sample_interpolated", __FILE__, __LINE__);
		return 0.0; // Invalid state
	}

	const LPCFile::LPCFrame &frame = lpc_file->get_frames()[frame_index];
	const LPCFile::LPCFrame &next_frame = lpc_file->get_frames()[next_frame_index];

	double output = excitation;

	// Interpolate coefficients and gain
	for (int i = 0; i < max_order; ++i) {
		double coeff = Math::lerp(frame.coefficients[i], next_frame.coefficients[i], frame_progress);
		output -= coeff * history[i];
	}

	// Update history with the new sample
	for (int i = max_order - 1; i > 0; --i) {
		history[i] = history[i - 1];
	}

	history[0] = output;
	double gain = Math::lerp(frame.gain, next_frame.gain, frame_progress);
	return output * std::sqrt(gain);
}

void LPCSynthesizer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("reset"), &LPCSynthesizer::reset);
	ClassDB::bind_method(D_METHOD("get_history"), &LPCSynthesizer::get_history);
	ClassDB::bind_method(D_METHOD("set_lpc_file", "file"), &LPCSynthesizer::set_lpc_file);
	ClassDB::bind_method(D_METHOD("get_lpc_file"), &LPCSynthesizer::get_lpc_file);
	ClassDB::bind_method(D_METHOD("set_max_order", "order"), &LPCSynthesizer::set_max_order);
	ClassDB::bind_method(D_METHOD("get_max_order"), &LPCSynthesizer::get_max_order);
	ClassDB::bind_method(D_METHOD("process_sample", "excitation", "frame_index"), &LPCSynthesizer::process_sample);
	ClassDB::bind_method(D_METHOD("process_sample_interpolated", "excitation", "frame_index", "next_frame_index", "frame_progress"), &LPCSynthesizer::process_sample_interpolated);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lpc_file", PROPERTY_HINT_RESOURCE_TYPE, "LPCFile"), "set_lpc_file", "get_lpc_file");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_order", PROPERTY_HINT_RANGE, "1,1000,1"), "set_max_order", "get_max_order");
}