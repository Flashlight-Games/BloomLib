#ifndef SPRITESUBDIVIDED
#define SPRITESUBDIVIDED

#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <string>

#include "Bloom/shaders/shaders.h" // SHADERS
#include "Bloom/textures/textures.h" // TEXTURES

namespace Bloom {
    class SubdividedSprite {
    public:

        SubdividedSprite() {};
        SubdividedSprite(const char* texture_path, int pixels_per_pixel, glm::mat4* view_matrix, glm::mat4 pixel_projection_matrix, glm::vec2 subdivision, glm::vec2 origin = glm::vec2(0.5, 0.5));
        ~SubdividedSprite();
        void Draw(float depth = 0.99);
        void DrawInstanced(int amount, float depth = 0.99);

        void SetLocation(glm::vec2 location) { transformation[0] = location[0]; transformation[1] = location[1]; UpdateLocation(); };
        glm::vec2 GetLocation() { return glm::vec2(transformation[0], transformation[1]); };

        void SetRotation(float rotation) { transformation[2] = rotation; UpdateLocation(); };
        float GetRotation() { return transformation[2]; };

        void SetScale(glm::vec2 scale) { transformation[3] = scale[0]; transformation[4] = scale[1]; UpdateLocation(); };
        glm::vec2 GetScale() { return glm::vec2(transformation[3], transformation[4]); };

        glm::vec2 GetSize() { return GetScale() * GetTexture()->GetSize(); };
        glm::vec2 GetOrigin() { return Origin; };

        void SetTransform(glm::vec2 location, float rotation, glm::vec2 scale)
        {
            transformation[0] = location[0]; transformation[1] = location[1];
            transformation[2] = rotation;
            transformation[3] = scale[0]; transformation[4] = scale[1];
            UpdateLocation();
        };

        Bloom::Texture2D* GetTexture() { return texture; };
        Bloom::Shader* GetShader() { return shader; };
        glm::mat4 GetModelMatrix() { return model_matrix; };

        void SetShader(Bloom::Shader* new_shader, glm::mat4 projection_matrix) {
            this->shader = new_shader; original_shader = false;

            shader->use();
            uColorUni = shader->getUniformId("uColor");
            uTextureUni = shader->getUniformId("uTexture");

            shader->uniformMat4(shader->getUniformId("view"), *view_matrix);
            shader->uniformMat4(shader->getUniformId("projection"), projection_matrix);
        };

        void SetVertexPositionOffset(int idx, glm::vec2 positionOffset);
        void SetVertexPositionOffsets(std::vector<glm::vec2>positionOffsets, int start_idx = 0);

        auto GetVerticesAmount() { return this->Subdivision + glm::vec2(1); };
    private:
        void UpdateLocation();

        glm::vec2 UVoffset;
        glm::vec2 UVscale;
        glm::vec2 Origin;
        glm::vec2 Subdivision;

        uint32_t vao;
        int ppp = 2; // pixels per pixel ( detail level )

        int indicesAmount = 6;

        float transformation[5] = {
            32,32, // Location
            0, // Rotation
            1,1, // Scale
        };

        uint32_t vbo; // Vertex buffer objects store verts on the GPU
        uint32_t ebo; // elemt array buffer tells the gpu what indices to connect
        Bloom::Shader* shader;
        Bloom::Texture2D* texture;
        glm::mat4 model_matrix;
        glm::mat4* view_matrix;

        bool original_shader;
        float uColorUni;
        float uTextureUni;

        uint8_t model_uid;
        uint8_t view_uid;
        uint8_t depth_uid;
    };
}

#endif
