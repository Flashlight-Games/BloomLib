#include <glad/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

export module Bloom.Sprite;

import Bloom.Shaders;
import Bloom.Textures;
import std;

export namespace Bloom {
    class Sprite {
    public:
        Sprite(
            const char* texture_path,
            float pixels_per_pixel,
            glm::mat4* view_matrix,
            glm::mat4* pixel_projection_matrix,
            Bloom::Shader* shader = nullptr,
            glm::vec2 origin = glm::vec2(0.5, 0.5),
            glm::vec2 size = glm::vec2(1, 1),
            glm::vec2 textureUVScale = glm::vec2(1, 1),
            glm::vec2 textureUVOffset = glm::vec2(0, 0)
        ) {
            Init(texture_path, pixels_per_pixel, view_matrix, pixel_projection_matrix, origin, size, textureUVScale, textureUVOffset);

            if (shader) {
                SetShader(shader, pixel_projection_matrix);
                original_shader = false;
            }
            else
                SetShader(new Bloom::Shader("Application/res/engine/shaders/sprite/spriteshader_vertex.glsl", "Application/res/engine/shaders/sprite/spriteshader_fragment.glsl"), pixel_projection_matrix);

            this->model_uid = this->shader->getUniformId("model");
            this->view_uid = this->shader->getUniformId("view");
            this->projection_uid = this->shader->getUniformId("projection");
            this->depth_uid = this->shader->getUniformId("custom_depth");
        }
        ~Sprite() {
            if (original_shader)
                delete shader;
            if (texture != nullptr)
                delete texture;

            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }

        void Draw(float depth = 0.99, bool shader_bound = false) {
            UpdateLocation();

            if (!shader_bound) {
                shader->use();
            }
            setTextureAtLocation(GL_TEXTURE0, this->texture->GetID()); shader->uniformInt(uTextureUni, 0);
            shader->uniformMat4(model_uid, model_matrix);
            shader->uniformMat4(view_uid, (*(view_matrix)));
            shader->uniformMat4(projection_uid, (*(projection_matrix)));
            shader->uniformFloat(depth_uid, depth);

            this->texture->bind();
            glBindVertexArray(this->vao);
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
        }

        void DrawInstanced(int amount, bool bind_texture = true, float depth = 0.99) {
            shader->use();
            if (bind_texture) {
                this->texture->bind();
                setTextureAtLocation(GL_TEXTURE0, this->texture->GetID()); shader->uniformInt(uTextureUni, 0);
            }
            shader->uniformMat4(view_uid, *(view_matrix));
            shader->uniformFloat(depth_uid, depth);

            UpdateLocation();

            glBindVertexArray(this->vao);
            glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0, amount);
        }
        void DrawInstanced(int amount, std::vector<GLuint> buffers_to_bind, float depth = 0.99) {
            shader->use();
            setTextureAtLocation(GL_TEXTURE0, this->texture->GetID()); shader->uniformInt(uTextureUni, 0);
            shader->uniformMat4(view_uid, *(view_matrix));
            shader->uniformFloat(depth_uid, depth);

            UpdateLocation();

            this->texture->bind();
            glBindVertexArray(this->vao);
            for (int i = 0; i < buffers_to_bind.size(); i++)
            {
                glBindBuffer(GL_ARRAY_BUFFER, buffers_to_bind.at(i));
            }
            glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0, amount);
        }

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

        void SetUVOffset(glm::vec2 offset) {
            float xScale = (this->texture->GetSize()[0] * ppp * UVscale[0]); // Sprite width 
            float yScale = (this->texture->GetSize()[1] * ppp * UVscale[1]); // Sprite width 
            float verts[] = {
                // POSITION                                          COLOR       Texture
                (Origin[0] - 1) * xScale,     (Origin[1] - 1) * yScale,      1,1,1,      offset[0],offset[1],
                (Origin[0]) * xScale, (Origin[1] - 1) * yScale,            1,1,1,      offset[0] + UVscale[0],offset[1],
                (Origin[0]) * xScale, (Origin[1]) * yScale,              1,1,1,      offset[0] + UVscale[0],offset[1] + UVscale[1],
                (Origin[0] - 1) * xScale,     (Origin[1]) * yScale,        1,1,1,      offset[0],offset[1] + UVscale[1]
            };
            glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the vbo as an array buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);
        }

        void SetInstanced(glm::mat4* projection_matrix) {
            SetShader(new Bloom::Shader("Application/res/engine/shaders/sprite/spriteshader_instanced_vertex.glsl", "Application/res/engine/shaders/sprite/spriteshader_instanced_fragment.glsl"), projection_matrix);
        }


        Bloom::Texture2D* GetTexture() { return texture; };
        Bloom::Shader* GetShader() { return shader; };
        glm::mat4 GetModelMatrix() { return model_matrix; };

        void SetShader(Bloom::Shader* new_shader, glm::mat4* projection_matrix, bool delete_shader = true) {
            const auto previous_shader = this->shader;
            this->shader = new_shader; original_shader = false;

            shader->use();
            uColorUni = shader->getUniformId("uColor");
            uTextureUni = shader->getUniformId("uTexture");
            this->model_uid = shader->getUniformId("model");
            this->view_uid = shader->getUniformId("view");
            this->depth_uid = shader->getUniformId("custom_depth");

            shader->uniformMat4(view_uid, *view_matrix);
            shader->uniformMat4(shader->getUniformId("projection"), *projection_matrix);

            if (delete_shader)
                delete previous_shader;
        };

        void SetTexture(Bloom::Texture2D& new_texture) { delete this->texture; this->texture = new Bloom::Texture2D(new_texture); }

        auto GetVAO() { return this->vao; }

    private:
        void Init(
            const char* texture_path,
            float pixels_per_pixel,
            glm::mat4* view_matrix,
            glm::mat4* pixel_projection_matrix,
            glm::vec2 origin = glm::vec2(0.5, 0.5),
            glm::vec2 size = glm::vec2(1, 1),
            glm::vec2 textureUVScale = glm::vec2(1, 1),
            glm::vec2 textureUVOffset = glm::vec2(0, 0)
        ) {
            this->ppp = pixels_per_pixel; this->view_matrix = view_matrix; this->projection_matrix = pixel_projection_matrix;
            this->UVoffset = textureUVOffset; this->UVscale = textureUVScale; this->Origin = origin;

            transformation[3] = size[0]; transformation[4] = size[1];

            glGenVertexArrays(1, &vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
            glBindVertexArray(vao);

            this->texture = new Bloom::Texture2D(texture_path, false);

            float xScale = (this->texture->GetSize()[0] * ppp * size[0]); // Sprite width 
            float yScale = (this->texture->GetSize()[1] * ppp * size[1]); // Sprite height 

            float verts[] = {
                // POSITION                                          COLOR       Texture                                                                   
                (origin[0] - 1) * xScale,     (origin[1] - 1) * yScale,      1,1,1,      textureUVOffset[0],textureUVOffset[1],
                (origin[0]) * xScale, (origin[1] - 1) * yScale,            1,1,1,      textureUVOffset[0] + textureUVScale[0],textureUVOffset[1],
                (origin[0]) * xScale, (origin[1]) * yScale,              1,1,1,      textureUVOffset[0] + textureUVScale[0],textureUVOffset[1] + textureUVScale[1],
                (origin[0] - 1) * xScale,     (origin[1]) * yScale,        1,1,1,      textureUVOffset[0],textureUVOffset[1] + textureUVScale[1],
            };

            glGenBuffers(1, &vbo); // Set vbo id
            glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the vbo as an array buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);


            glGenBuffers(1, &ebo); // Set ebo id
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // bind the ebo as an element array buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(0 * sizeof(float)))); // Position

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(2 * sizeof(float)))); // Color

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(5 * sizeof(float)))); // Texture

            model_matrix = glm::mat4(1.0);
            model_matrix = glm::translate(model_matrix, glm::vec3(this->transformation[0], this->transformation[1], 0.0f));
            model_matrix = glm::rotate(model_matrix, (float)glm::radians(this->transformation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(this->transformation[3], this->transformation[4], 1.0f));
        }
        void UpdateLocation() {
            model_matrix = glm::mat4(1.0);
            model_matrix = glm::translate(model_matrix, glm::vec3(this->transformation[0], this->transformation[1], 0.0f));
            model_matrix = glm::rotate(model_matrix, (float)glm::radians(this->transformation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(this->transformation[3], this->transformation[4], 1.0f));
        }

        glm::vec2 UVoffset;
        glm::vec2 UVscale;
        glm::vec2 Origin;

        uint32_t vao;
        float ppp = 2; // pixels per pixel ( detail level )

        int indices[6] = {
            0,1,2,
            0,2,3
        };

        float transformation[5] = {
            0,0, // Location
            0, // Rotation
            1,1, // Scale
        };

        uint32_t vbo; // Vertex buffer objects store verts on the GPU
        uint32_t ebo; // elemt array buffer tells the gpu what indices to connect
        Bloom::Shader* shader;
        Bloom::Texture2D* texture;
        glm::mat4 model_matrix;
        glm::mat4* view_matrix;
        glm::mat4* projection_matrix;

        bool original_shader;
        float uColorUni;
        float uTextureUni;

        uint8_t model_uid;
        uint8_t view_uid;
        uint8_t projection_uid;
        uint8_t depth_uid;
    };
}
