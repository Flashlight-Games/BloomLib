#ifndef BLOOM_SHAKE
#define BLOOM_SHAKE


#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>

namespace Bloom{
    class Shake{
        public:
            Shake(float frequency, float amplitude, float duration, int randomness = 0,bool continuous = false);
            void Start(float time) { this->time = time; active = true; };
            void Stop(){ active = false; };
            float GetValue(float time);

        private:
            float time;
            float freq;
            float ampl;
            float dura;
            float rnd;
            bool cont;
            bool active;
    };
}

#endif