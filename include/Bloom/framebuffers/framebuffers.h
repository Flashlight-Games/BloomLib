#ifndef BLOOM_FRAMEBUFFERS
#define BLOOM_FRAMEBUFFERS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include "../shaders/shaders.h"
#include "../textures/textures.h"

namespace Bloom{
    class Framebuffer{
        public:
        Framebuffer(glm::vec2 size ,bool linear_scaling = true, bool HDR = false);
        ~Framebuffer();
        // Framebuffer(){};
        
        Shader fbShader = Shader("include/Bloom/res/engine/shaders/framebuffer/framebuffer_vertex.glsl","include/Bloom/res/engine/shaders/framebuffer/framebuffer_fragment_alpha.glsl");
        void use(bool clear);
        virtual void draw();
        virtual void drawToDefault(glm::vec2 window_size,bool clear = true,glm::vec2 offset = glm::vec2(0));

        uint16_t getWidth(){return width;};
        uint16_t getHeight(){return height;};
        uint16_t getVBO(){return fbVbo;};
        uint16_t getVBA(){return fbVao;};
        uint16_t getFBO(){return fbo;};
        Texture2D* getTexture(){return fbTexture;};
        void SetTexture(Texture2D* texture) {
            fbTexture = texture;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture->GetID(), 0);
            width = fbTexture->GetSize().x;
            height = fbTexture->GetSize().y;
        };
        uint16_t getRBO(){return rbo;};

        Bloom::Shader GetShader() { return fbShader; };

        void setShader(const char* fragment_source);
        void setTextureUniform(const char* uniform);

        void SetLocationOffset(glm::vec2 offset);

        private:
        uint16_t width;
        uint16_t height;

        uint32_t fbVao;
        uint32_t fbVbo;

        uint32_t fbo;
        Texture2D* fbTexture;
        uint32_t rbo;

        glm::vec2 offset = glm::vec2(0);
    };
}

void bindDefaultFramebuffer(bool clear,int width = -1, int height = -1, glm::vec2 offset = glm::vec2(0,0));

#endif