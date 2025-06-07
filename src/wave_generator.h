#ifndef TTS_GD_WAVE_GENERATOR_H
#define TTS_GD_WAVE_GENERATOR_H

#include <random>
#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

    class WaveGenerator : public RefCounted {
        GDCLASS(WaveGenerator, RefCounted);
    
        private:
            /**
             * Pink noise coefficients for the IIR filter.
             * These coefficients are derived from the Voss-McCartney algorithm.
             */
            double bN[6] = {0.0};
            double stage_multiplier = 0.0;
            uint32_t stages = 6;
            
            std::mt19937 generator;
            std::uniform_real_distribution<double> distribution;
        

        protected:
            static void _bind_methods();
        
        public:
            WaveGenerator();
            ~WaveGenerator();

            void reset();
            double next_white_noise();
            double next_pink_noise();
            double voiced_wave(double phase, double shape);

            void set_seed(uint32_t seed);


            void set_stages(uint32_t octaves);
            uint32_t get_stages() const;
    
}; 

}


#endif // TTS_GD_WAVE_GENERATOR_H
