#ifndef SPRITE
#define SPRITE

#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <string>

#include "../shaders/shaders.h" // SHADERS
#include "../textures/textures.h" // TEXTURES

namespace Bloom{
    class Sprite{
        public:
            
            Sprite(){};
            Sprite(const char* texture_path,int pixels_per_pixel, glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix, glm::vec2 origin = glm::vec2(0.5,0.5),glm::vec2 size = glm::vec2(1,1),glm::vec2 textureUVScale = glm::vec2(1,1),glm::vec2 textureUVOffset = glm::vec2(0,0));
            void Draw(glm::mat4 view_matrix);

            void SetLocation(glm::vec2 location){transformation[0] = location[0];transformation[1] = location[1];UpdateLocation();};
            glm::vec2 GetLocation(){return glm::vec2(transformation[0],transformation[1]);};

            void SetRotation(float rotation){transformation[2]=rotation;UpdateLocation();};
            float GetRotation(){return transformation[2];};

            void SetScale(glm::vec2 scale){transformation[3] = scale[0];transformation[4] = scale[1];UpdateLocation();};
            glm::vec2 GetScale(){return glm::vec2(transformation[3],transformation[4]);};

            void SetTransform(glm::vec2 location, float rotation, glm::vec2 scale)
            {transformation[0] = location[0];transformation[1] = location[1];
            transformation[2]=rotation;
            transformation[3] = scale[0];transformation[4] = scale[1];};

            void SetUVOffset(glm::vec2 offset);

            Bloom::Texture2D GetTexture(){return texture;};
            Bloom::Shader GetShader(){return shader;};
            glm::mat4 GetModelMatrix(){return model_matrix;};


        private:
            void UpdateLocation();

            glm::vec2 UVoffset;
            glm::vec2 UVscale;
            glm::vec2 Origin;

            uint32_t vao;
            int ppp = 2; // pixels per pixel ( detail level )
            
            int indices[6] = {
                0,1,2,
                0,2,3
            };

            float transformation[5] = {
                32,32, // Location
                0, // Rotation
                1,1, // Scale
            };

            uint32_t vbo; // Vertex buffer objects store verts on the GPU
            uint32_t ebo; // elemt array buffer tells the gpu what indices to connect
            Bloom::Shader shader = Bloom::Shader("src/sprite/shaders/spriteshader_vertex.glsl","src/sprite/shaders/spriteshader_fragment.glsl");
            Bloom::Texture2D texture = nullptr;
            glm::mat4 model_matrix;
            glm::mat4* view_matrix;

            float uColorUni;
            float uTextureUni;
    };
}

#endif