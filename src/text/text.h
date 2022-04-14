#ifndef BLOOM_TEXT
#define BLOOM_TEXT

#include "STBTT/stb_truetype.h"
#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include "../textures/textures.h"
#include "../shaders/shaders.h"

#include <iostream>

namespace Bloom{
    class Text{
        public:
        Text(const char* path, float size,float spacing = -0.5);
        void Print(glm::vec2 position, std::string text,glm::vec2 screen_size,glm::vec4 color = glm::vec4(1,1,1,1));

        const char* getPath(){return path;};
        float getSize(){return size;};
        Texture2D getTexture(){return texture;};

        private:
        const char* path;
        float size;

        unsigned char ttf_buffer[1<<20];

        stbtt_bakedchar char_data[96]; // all characters
        Texture2D texture = NULL;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint bitsize;
        float spacing = 1.9;
        Shader qShader = Shader("res/shaders/font/font_vertex.glsl","res/shaders/font/font_fragment.glsl");
        int indices[6] = {
            0,1,2,
            0,2,3
        };
    };
}
#endif