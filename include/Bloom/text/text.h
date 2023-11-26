#ifndef BLOOM_TEXT
#define BLOOM_TEXT

#include "STBTT/stb_truetype.h"
#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include "Bloom/textures/textures.h"
#include "Bloom/shaders/shaders.h"

#include <iostream>

namespace Bloom{
    class Text{
        public:
            Text(const char* path,glm::mat4* view_matrix, glm::mat4 projection_matrix,float spacing = -0.5, uint16_t resolution = 128);
            ~Text();
            void Print(glm::vec2 position, std::string text,float size,glm::vec2 screen_size,glm::vec4 color = glm::vec4(1,1,1,1),bool centered = false, float depth = 0.99);
        
            const char* GetPath(){return path;};
            float GetResolution(){return resolution;};
            Texture2D* GetTexture(){return texture;};

        private:
            glm::vec2 GetCenterOffset(std::string text, float size, glm::vec2 screen_size);

            const char* path;
            float resolution;

            unsigned char ttf_buffer[1<<20];

            stbtt_fontinfo font;
            stbtt_bakedchar char_data[96]; // all characters
            Texture2D* texture;
            GLuint vao;
            GLuint vbo;
            GLuint ebo;
            GLuint bitsize;
            float spacing = 1.9;
            Shader qShader = Shader("include/Bloom/res/engine/shaders/font/font_vertex.glsl","include/Bloom/res/engine/shaders/font/font_fragment.glsl");
            int indices[6] = {
                0,1,2,
                0,2,3
            };
            glm::mat4* view_matrix;
            glm::mat4* projection_matrix;
            float scale = 1;

            uint8_t color_uid;
            uint8_t model_uid;
            uint8_t view_uid;
            uint8_t projection_uid;
            uint8_t depth_uid;
    };
}
#endif