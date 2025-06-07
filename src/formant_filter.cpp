#include "formant_filter.h"


using namespace godot;


void FormantFilter::Processor::set_filter(FormantFilter *p_filter, bool p_clear_history) {
	if (!p_filter) {
		UtilityFunctions::push_error("FormantFilterProcessor::set_filter: p_filter is null");
		return;
	}
	filter = p_filter;
	if (p_clear_history) {
		ha1 = 0.0f;
		ha2 = 0.0f;
		hb1 = 0.0f;
		hb2 = 0.0f;
	}
	update_coeffs();
}

void FormantFilter::Processor::update_coeffs(int p_interp_buffer_len) {
	if (!filter) {
		UtilityFunctions::push_error("FormantFilterProcessor::update_coeffs: filter is null");
		return;
	}

	if (p_interp_buffer_len) {
		Coeffs old_coeffs = coeffs;
		filter->prepare_coefficients(&coeffs);
		incr_coeffs.a1 = (coeffs.a1 - old_coeffs.a1) / p_interp_buffer_len;
		incr_coeffs.a2 = (coeffs.a2 - old_coeffs.a2) / p_interp_buffer_len;
		incr_coeffs.b0 = (coeffs.b0 - old_coeffs.b0) / p_interp_buffer_len;
		incr_coeffs.b1 = (coeffs.b1 - old_coeffs.b1) / p_interp_buffer_len;
		incr_coeffs.b2 = (coeffs.b0 - old_coeffs.b2) / p_interp_buffer_len;
		coeffs = old_coeffs;
	} else {
		filter->prepare_coefficients(&coeffs);
	}
}


void FormantFilter::Processor::process(float *p_samples, int p_amount, int p_stride, bool p_interpolate) {
	if (!filter) {
		UtilityFunctions::push_error("FormantFilterProcessor::process: filter is null");
		return;
	}

	if (p_interpolate) {
		for (int i = 0; i < p_amount; i++) {
			process_one_interp(*p_samples);
			p_samples += p_stride;
		}
	} else {
		for (int i = 0; i < p_amount; i++) {
			process_one(*p_samples);
			p_samples += p_stride;
		}
	}
}

void FormantFilter::Processor::set_type(FilterType p_type) {
	if (type == p_type) {
		return;
	}
	type = p_type;
	ha1 = 0.0f;
	ha2 = 0.0f;
	hb1 = 0.0f;
	hb2 = 0.0f;
}

FormantFilter::Processor::Processor() {
	coeffs.a1 = 0.0;
	coeffs.a2 = 0.0;
	coeffs.b0 = 0.0;
	coeffs.b1 = 0.0;
	coeffs.b2 = 0.0;

	incr_coeffs.a1 = 0.0;
	incr_coeffs.a2 = 0.0;
	incr_coeffs.b0 = 0.0;
	incr_coeffs.b1 = 0.0;
	incr_coeffs.b2 = 0.0;

}

FormantFilter::FormantFilter() {
	for (int j = 0; j < 4; j++) {
		processors[j].set_filter(this);
	}
	
}

void FormantFilter::set_type(FilterType p_type) {
	if (type == p_type) {
		return;
	}
	type = p_type;
	for (int j = 0; j < 4; j++) {
		processors[j].set_type(p_type);
		processors[j].update_coeffs();
	}
	
}

void FormantFilter::set_stages(FilterStages p_stages) {
	stages = p_stages;
}

void FormantFilter::set_cutoff(float p_cutoff) {
	cutoff = p_cutoff;
}
void FormantFilter::set_resonance(float p_resonance) {
	resonance = p_resonance;
}
void FormantFilter::set_amplification_gain(float p_gain) {
	amplification_gain = p_gain;
}
void FormantFilter::set_sampling_rate(float p_sampling_rate) {
	sampling_rate = p_sampling_rate;
}
void FormantFilter::set_filter_gain(float p_gain) {
	filter_gain = p_gain;
}
float FormantFilter::get_cutoff() const {
	return cutoff;
}
float FormantFilter::get_resonance() const {
	return resonance;
}
float FormantFilter::get_amplification_gain() const {
	return amplification_gain;
}
float FormantFilter::get_sampling_rate() const {
	return sampling_rate;
}
float FormantFilter::get_filter_gain() const {
	return filter_gain;
}
FormantFilter::FilterType FormantFilter::get_type() const {
	return type;
}
FormantFilter::FilterStages FormantFilter::get_stages() const {
	return stages;
}

void FormantFilter::reset() {
	for (int j = 0; j < 4; j++) {
		processors[j].set_filter(this, true);
	}
	
}

void FormantFilter::prepare_coefficients(Processor::Coeffs *p_coeffs) const {
	if (!p_coeffs) {
		UtilityFunctions::push_error("FormantFilter::prepare_coefficients: p_coeffs is null");
		return;
	}

	int sr_limit = (sampling_rate / 2.0f) + 512;

	double final_cutoff = (cutoff > sr_limit) ? sr_limit : cutoff;
	if (final_cutoff < 1.0) {
		final_cutoff = 1.0;
	}

	// Convert cutoff frequency to radians per sample
	double omega = Math_TAU * final_cutoff / sampling_rate;

	double sin_v = Math::sin(omega);
	double cos_v = Math::cos(omega);

	double Q = resonance;
	if (Q <= 0.0) {
		// Prevent division by zero
		Q = 0.0001;
	}

	double tmp_gain = filter_gain;

	if (type == BANDPASS) {
		Q *= 2.0;
		// Gain is not used in bandpass filters
		tmp_gain = 1.0;
	}


	if (tmp_gain < 0.001) {
		tmp_gain = 0.001;
	}

	if (stages > 1) {
		Q = (Q > 1.0 ? Math::pow(Q, 1.0 / stages) : Q);
		tmp_gain = Math::pow(tmp_gain, 1.0 / (stages + 1));
	}

	double alpha = sin_v / (2.0 * Q);

	double a0 = 1.0 + alpha;

	switch (type) {
		case BANDPASS: {
			p_coeffs->b0 = alpha * std::sqrt(Q + 1);
			p_coeffs->b1 = 0.0;
			p_coeffs->b2 = -alpha * std::sqrt(Q + 1);
			p_coeffs->a1 = -2.0 * cos_v;
			p_coeffs->a2 = 1.0 - alpha;
		} break;

		case NOTCH: {
			p_coeffs->b0 = 1.0;
			p_coeffs->b1 = -2.0 * cos_v;
			p_coeffs->b2 = 1.0;
			p_coeffs->a1 = -2.0 * cos_v;
			p_coeffs->a2 = 1.0 - alpha;
		} break;
	}

	p_coeffs->b0 /= a0;
	p_coeffs->b1 /= a0;
	p_coeffs->b2 /= a0;
	p_coeffs->a1 /= -a0;
	p_coeffs->a2 /= -a0;

}




template<int S>
void FormantFilter::_apply_processors(float *p_src_samples, float *p_dst_samples, int p_amount) {
	for (int i = 0; i < p_amount; i++) {
		float f = p_src_samples[i];
		processors[0].process_one(f);
		if constexpr (S > 1) {
			processors[1].process_one(f);
		}
		if constexpr (S > 2) {
			processors[2].process_one(f);
		}
		if constexpr (S > 3) {
			processors[3].process_one(f);
		}

		p_dst_samples[i] = f;
	}
}

template<int S>
void FormantFilter::_apply_processors_interpolated(float *p_src_samples, float *p_dst_samples, int p_amount) {
	for (int i = 0; i < p_amount; i++) {
		float f = p_src_samples[i];
		processors[0].process_one_interp(f);
		if constexpr (S > 1) {
			processors[1].process_one_interp(f);
		}
		if constexpr (S > 2) {
			processors[2].process_one_interp(f);
		}
		if constexpr (S > 3) {
			processors[3].process_one_interp(f);
		}
		p_dst_samples[i] = f;
	}
}

void FormantFilter::process(float *p_samples, int p_amount) {
	process_interpolated(p_samples, p_amount, 0);
}

void FormantFilter::process_interpolated(float *p_samples, int p_amount, int p_interp_buffer_len) {
	if (p_amount <= 0) {
		return;
	}
	for (int i = 0; i < 4; i++) {
		processors[i].update_coeffs(p_interp_buffer_len);
	}

	int stage_count = int(stages) + 1;
	if (stage_count == 1) {
		_apply_processors_interpolated<1>(p_samples, p_samples, p_amount);
	} else if (stage_count == 2) {
		_apply_processors_interpolated<2>(p_samples, p_samples, p_amount);
	} else if (stage_count == 3) {
		_apply_processors_interpolated<3>(p_samples, p_samples, p_amount);
	} else if (stage_count == 4) {
		_apply_processors_interpolated<4>(p_samples, p_samples, p_amount);
	}
	if (amplification_gain != 0.0f) {
		float gain_factor = Math::pow(10.0f, amplification_gain / 20.0f);
		for (int i = 0; i < p_amount; i++) {
			p_samples[i] = Math::clamp(p_samples[i] * gain_factor, -1.0f, 1.0f);
		}
	}
}

void FormantFilter::process_buffer(PackedFloat32Array p_samples) {
	if (p_samples.size() <= 0) {
		return;
	}
	process(const_cast<float *>(p_samples.ptr()), p_samples.size());
}

void FormantFilter::process_buffer_interpolated(PackedFloat32Array p_samples, int p_interp_buffer_len) {
	if (p_samples.size() <= 0) {
		return;
	}
	process_interpolated(const_cast<float *>(p_samples.ptr()), p_samples.size(), p_interp_buffer_len);
}



void FormantFilter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_type", "type"), &FormantFilter::set_type);
	ClassDB::bind_method(D_METHOD("set_stages", "stages"), &FormantFilter::set_stages);
	ClassDB::bind_method(D_METHOD("set_cutoff", "cutoff"), &FormantFilter::set_cutoff);
	ClassDB::bind_method(D_METHOD("set_resonance", "resonance"), &FormantFilter::set_resonance);
	ClassDB::bind_method(D_METHOD("set_amplification_gain", "gain"), &FormantFilter::set_amplification_gain);
	ClassDB::bind_method(D_METHOD("set_sampling_rate", "sampling_rate"), &FormantFilter::set_sampling_rate);
	ClassDB::bind_method(D_METHOD("set_filter_gain", "gain"), &FormantFilter::set_filter_gain);

	ClassDB::bind_method(D_METHOD("get_cutoff"), &FormantFilter::get_cutoff);
	ClassDB::bind_method(D_METHOD("get_resonance"), &FormantFilter::get_resonance);
	ClassDB::bind_method(D_METHOD("get_amplification_gain"), &FormantFilter::get_amplification_gain);
	ClassDB::bind_method(D_METHOD("get_sampling_rate"), &FormantFilter::get_sampling_rate);
	ClassDB::bind_method(D_METHOD("get_filter_gain"), &FormantFilter::get_filter_gain);
	ClassDB::bind_method(D_METHOD("get_stages"), &FormantFilter::get_stages);
	ClassDB::bind_method(D_METHOD("get_type"), &FormantFilter::get_type);

	ClassDB::bind_method(D_METHOD("reset"), &FormantFilter::reset);

	ClassDB::bind_method(D_METHOD("process_buffer", "samples"), &FormantFilter::process_buffer);
	ClassDB::bind_method(D_METHOD("process_buffer_interpolated", "samples", "interp_buffer_len"), &FormantFilter::process_buffer_interpolated, DEFVAL(0));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, "Bandpass,Notch"), "set_type", "get_type");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "stages", PROPERTY_HINT_ENUM, "1,2,3,4"), "set_stages", "get_stages");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cutoff", PROPERTY_HINT_RANGE, "0.0,100000.0,0.01"), "set_cutoff", "get_cutoff");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "resonance", PROPERTY_HINT_RANGE, "0.0,100.0,0.01"), "set_resonance", "get_resonance");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplification_gain", PROPERTY_HINT_RANGE, "-100.0,100.0,0.01"), "set_amplification_gain", "get_amplification_gain");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sampling_rate", PROPERTY_HINT_RANGE, "0.0,100000.0,0.01"), "set_sampling_rate", "get_sampling_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "filter_gain", PROPERTY_HINT_RANGE, "-100.0,100.0,0.01"), "set_filter_gain", "get_filter_gain");
	
	BIND_ENUM_CONSTANT(BANDPASS);
    BIND_ENUM_CONSTANT(NOTCH);
    BIND_ENUM_CONSTANT(STAGE_1);
    BIND_ENUM_CONSTANT(STAGE_2);
    BIND_ENUM_CONSTANT(STAGE_3);
    BIND_ENUM_CONSTANT(STAGE_4);
}	