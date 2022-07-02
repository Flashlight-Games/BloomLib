#ifndef BLOOM_PARTICLES
#define BLOOM_PARTICLES

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <vector>
#include "sprite/sprite.h"

namespace Bloom{
    class Particle {
    public:
        Particle(int instance, float time);
        virtual void Update(float Gametime, float Gamespeed);

        glm::vec2 GetLocation() { return location; };
        float GetRotation() { return rotation; };
        glm::vec2 GetScale() { return scale; };
        glm::mat4 GetTransformMatrix();
        glm::vec4 GetColor() { return color; };
        float GetLifetime() { return lifetime; };
        float GetAge() { return age; };

        void SetLocation(glm::vec2 t_location) { location = t_location; };
        void SetRotation(float t_rotation) { rotation = t_rotation; };
        void SetScale(glm::vec2 t_scale) { scale = t_scale; };
        void SetColor(glm::vec4 t_color) { color = t_color; };
        void SetLifetime(float t_lifetime) { lifetime = t_lifetime; };
        void SetAge(float game_time) { age = game_time - spawn_time; };
    private:
        float spawn_time = 0;
        glm::vec2 location = glm::vec2(0);
        float rotation = 0;
        glm::vec2 scale = glm::vec2(1);
        float age = 0;
        float lifetime = 1;
        glm::vec4 color = glm::vec4(1);
        int id = 0;
    };

    template <class T>
    class ParticleEmitter{
    public:
        ParticleEmitter(int amount, glm::mat4 view_matrix, glm::mat4 projection_matrix,std::string fShader = "") {
            p_amount = amount;
            p_view_matrix = view_matrix;
            p_projection_matrix = projection_matrix;
            p_sprite = new Bloom::Sprite("res/engine/textures/rect16.png", 1, p_view_matrix, p_projection_matrix, glm::vec2(0.5, 0.5), glm::vec2(1));
            if (fShader != "") {
                p_sprite->SetShader(new Bloom::Shader("res/engine/shaders/particle/particleshader_vertex.glsl", fShader.c_str()), view_matrix, projection_matrix);
            }
            else {
                p_sprite->SetShader(new Bloom::Shader("res/engine/shaders/particle/particleshader_vertex.glsl", "res/engine/shaders/particle/particleshader_fragment.glsl"),view_matrix,projection_matrix);
            }
        };
        std::vector<T*> Play(GLFWwindow* window, float time,glm::vec2 location = glm::vec2(0,0)) {
            int last_size = particles.size();
            time = std::max(time, (float)0.0);
            std::vector<T*> new_particles;

            for (int i = 0; i < p_amount; i++)
            {
                T* new_particle = new T(i + last_size, time);
                new_particle->SetLocation(location);
                particles.push_back(new_particle);
                new_particles.push_back(new_particle);
            }
            return new_particles;
        };
        void Draw(float Gametime, float Gamespeed) {
            if (particles.size() > 0) {
                for (int i = 0; i < particles.size(); i++)
                {
                    T* in = particles.at(i);
                    Particle* p = static_cast<Particle*>(in);
                    p->Update(Gametime, Gamespeed);
                    p_sprite->GetShader()->use();
                    p_sprite->GetShader()->uniformMat4(p_sprite->GetShader()->getUniformId(("model[" + std::to_string(i % 101) + "]").c_str()), p->GetTransformMatrix());
                    p_sprite->GetShader()->uniformVec4(p_sprite->GetShader()->getUniformId(("color[" + std::to_string(i % 101) + "]").c_str()), p->GetColor());

                    if(i > 0 && i % 101 == 100)
                        p_sprite->DrawInstanced(p_view_matrix, (int)std::min((int)particles.size(), 100));

                    if (p->GetAge() > p->GetLifetime())
                        particles.erase(particles.begin() + i);
                    else {
                        p->SetAge(Gametime);
                    }
                }
                
                p_sprite->DrawInstanced(p_view_matrix, (int)std::min((int)particles.size(), (int)particles.size() % 101));
                
            }
        };

        void Clear() {
            particles.clear();
        }

        std::vector<T*> GetParticles() { return particles; };
    private:
        glm::mat4 p_view_matrix;
        glm::mat4 p_projection_matrix;
        int p_amount;
        std::vector<T*> particles;
        Bloom::Sprite* p_sprite;
    };
}

#endif