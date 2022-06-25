#ifndef P_HURT
#define P_HURT

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <vector>

#include "../src/particles/particles.h"

namespace Bloom{
    class Particle_HURT : public Bloom::Particle {
        using Bloom::Particle::Particle;
    public:
        Particle_HURT(int instance, float time);

        void Update(float Gametime, float Gamespeed);
        void SetInitialVelocity(glm::vec2 t_velocity) { initial_velocity = t_velocity; };
    private:
        glm::vec2 initial_velocity = glm::vec2(0,0);
    };
}

#endif