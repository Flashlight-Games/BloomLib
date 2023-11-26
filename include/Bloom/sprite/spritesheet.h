#ifndef SPRITESHEET
#define SPRITESHEET

#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <string>

#include <chrono>

#include "../shaders/shaders.h" // SHADERS
#include "../textures/textures.h" // TEXTURES

#include "sprite.h"

namespace Bloom{
    class Spritesheet{
        public:
        Spritesheet(const char* texture_path,int pixels_per_pixel,glm::vec2 tiling, glm::mat4* view_matrix, glm::mat4 pixel_projection_matrix, glm::vec2 origin = glm::vec2(0.5,0.5));
        void Draw(float depth = 0.99);
        int Advance(int step = 1);
        int TimedAdvance(float duration,int step = 1);
        void SetIdx(int idx);

        void SetScale(glm::vec2 scale) { this->sprite->SetScale(scale); };
        void SetLocation(glm::vec2 location) { this->sprite->SetLocation(location); };
        void SetRotation(float rotation) { this->sprite->SetRotation(rotation); };

        Sprite* GetSprite(){return sprite;};
        glm::vec2 GetTiling(){return tiling;};

        private:
        Sprite* sprite;
        glm::vec2 tiling;
        int idx = 0;
        std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    };
}

#endif