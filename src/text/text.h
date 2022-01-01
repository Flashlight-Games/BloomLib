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
        Text(const char* path, float size);
        void Print(float x, float y, std::string text);

        const char* getPath(){return path;};
        float getSize(){return size;};
        Texture2D getTexture(){return texture;};

        private:
        const char* path;
        float size;

        unsigned char ttf_buffer[1<<20];
        unsigned char temp_bitmap[256*256];

        stbtt_bakedchar char_data[96]; // all characters
        Texture2D texture = NULL;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        Shader qShader = Shader("src/shaders/font/font_vertex.glsl","src/shaders/font/font_fragment.glsl");

        int indices[6] = {
            0,1,2,
            0,2,3
        };
    };
}
#endif