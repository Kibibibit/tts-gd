
#include "wave_generator.h"
#include <cmath>
#include <vector>
#include <random>


using namespace godot;

static const double PINK_NOISE_RETENTION_FACTORS[6] = {
    0.99886,
    0.99332,
    0.96900,
    0.86650,
    0.55000,
    -0.7616,
};
static const double PINK_NOISE_SCALING_FACTORS[6] = {
    0.0555179,
    0.0750759,
    0.1538520,
    0.3104856,
    0.5329522,
    -0.0168980,
};

WaveGenerator::WaveGenerator() {
    reset();
    stage_multiplier = 0.25 / sqrtf64(stages);

}
WaveGenerator::~WaveGenerator() {}

void WaveGenerator::reset() {
    // Initialize the pink noise coefficients
    for (int i = 0; i < 6; ++i) {
        bN[i] = 0.0;
    }
    this->generator = std::mt19937(std::random_device{}());
    this->distribution = std::uniform_real_distribution<double>(0.0, 1.0);

}

double WaveGenerator::voiced_wave(double phase, double shape) {
    double t = fmod(phase, 1.0);
    if (t < shape) {
        // This is the slow, rising part of the wave.
        // If shape <= 0, this section will not happen.
        double t_prime = t / shape;
        return t_prime * t_prime;
    } else {
        // The fast falling part of the wave.
        // If shape >= 1, this section will not happen.
        double t_prime = (t - shape) / (1.0 - shape);
        return 1.0 - t_prime * t_prime;
    }
}

double WaveGenerator::bisqwit(int step, double rate, double pitch, double breath, double buzz) {
    double p = pitch; // TODO: Potentially apply hysteresis to pitch

    double w = (step %= unsigned(rate/p)) / double(rate/p);
    return  (-0.5 + next_pink_noise())*breath + (std::exp2(w)-1.0/(1.0+w))*buzz;
}

double WaveGenerator::next_white_noise() {
    return distribution(generator);
}

double WaveGenerator::next_pink_noise() {
    // Voss-McCartney algorithm for pink noise generation
    double white = next_white_noise();
    double total = 0.0;
    for (int i = 0; i < this->stages; ++i) {
        bN[i] = PINK_NOISE_RETENTION_FACTORS[i] * bN[i] + white * PINK_NOISE_SCALING_FACTORS[i];
        total += bN[i];
    }
    return total * stage_multiplier;
}

void WaveGenerator::set_seed(uint32_t seed) {
    generator.seed(seed);
}

void WaveGenerator::set_stages(uint32_t octaves) {
    this->stages = octaves;
    stage_multiplier = 0.25 / sqrtf64(stages);
}
uint32_t WaveGenerator::get_stages() const {
    return this->stages;
}

void WaveGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("reset"), &WaveGenerator::reset);
    ClassDB::bind_method(D_METHOD("next_white_noise"), &WaveGenerator::next_white_noise);
    ClassDB::bind_method(D_METHOD("next_pink_noise"), &WaveGenerator::next_pink_noise);
    ClassDB::bind_method(D_METHOD("voiced_wave", "phase", "shape"), &WaveGenerator::voiced_wave);
    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &WaveGenerator::set_seed);
    ClassDB::bind_method(D_METHOD("set_stages", "octaves"), &WaveGenerator::set_stages);
    ClassDB::bind_method(D_METHOD("get_stages"), &WaveGenerator::get_stages);
    ClassDB::bind_method(D_METHOD("bisqwit", "step", "rate", "pitch", "breath", "buzz"), &WaveGenerator::bisqwit);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "stages", PROPERTY_HINT_RANGE, "1,6,1"), "set_stages", "get_stages");
}