#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

export module Bloom.Framebuffers;

import Bloom.Shaders;
import Bloom.Textures;

export void bindDefaultFramebuffer(bool clear, int width = -1, int height = -1, glm::vec2 offset = glm::vec2(0, 0), float zoom = 1.0) {
    if (width && height) {
        float newWidth = width * (zoom);
        float newHeight = height * (zoom);

        float newOffsetX = offset.x + (width - newWidth) / 2;
        float newOffsetY = offset.y + (height - newHeight) / 2;

        glViewport(newOffsetX, newOffsetY, newWidth, newHeight);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (clear) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

export namespace Bloom {
    class Framebuffer {
    public:
        Framebuffer(glm::vec2 size) : size(size){
            initFramebuffer();
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            AddDepthStencilComponent();

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("GL_FRAMEBUFFER NOT COMPLETE!");
        }
        Framebuffer(glm::vec2 size, bool linear_scaling) : size(size) {
            initFramebuffer();
            AddTextureComponent(new Texture2D(NULL, size, linear_scaling, GL_RGBA));

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("GL_FRAMEBUFFER NOT COMPLETE!");

            glBindFramebuffer(GL_FRAMEBUFFER, 0); // BIND RENDER BUFFER
            setTextureUniform("uTexture");
        }
        ~Framebuffer() {
            if (fbTexture != nullptr) {
                delete this->fbTexture;
            }

            glDeleteVertexArrays(1, &fbVao);
            glDeleteBuffers(1, &fbVbo);
            glDeleteFramebuffers(1, &fbo);
            glDeleteRenderbuffers(1, &rbo);
        }

        void use(bool clear = false, glm::vec2 offset = glm::vec2(0), float zoom = 1.0f) {
            if (clear)
                return this->use((int)(this->attached_textures.size()), offset, zoom);
            return this->use(0, offset, zoom);
        }
        void use(int clear_range, glm::vec2 offset = glm::vec2(0), float zoom = 1.0f) {
            std::vector<int> clear_idxs = {};
            for (int i = 0; i < clear_range; i++) {
                clear_idxs.push_back(i);
            }
            return this->use(clear_idxs, offset, zoom);
        }
        void use(std::vector<int> clear_idx, glm::vec2 offset = glm::vec2(0), float zoom = 1.0f) {
            float newWidth = size.x * (zoom);
            float newHeight = size.y * (zoom);

            float newOffsetX = offset.x + (size.x - newWidth) / 2;
            float newOffsetY = offset.y + (size.y - newHeight) / 2;

            glViewport(newOffsetX, newOffsetY, newWidth, newHeight);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glDrawBuffers(color_attachments, attached_buffers.data());

            fbShader.use();

            const GLfloat clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            const GLfloat clearDepth = 1.0f;

            for (int i = 0; i < clear_idx.size(); i++) {
                glClearBufferfv(GL_COLOR, clear_idx.at(i), clearColor);
                glClearBufferfv(GL_DEPTH, clear_idx.at(i), &clearDepth);
            }
        }
        virtual void draw(float depth = 0.99, std::vector<Bloom::Texture2D*> extra_textures = {}) {
            fbShader.use();
            fbShader.uniformMat4(offset_uid, (glm::translate(glm::mat4(1.0), glm::vec3(this->offset.x, this->offset.y, 0.0f))));
            fbShader.uniformFloat(depth_uid, depth);

            if (fbTexture) {
                glActiveTexture(GL_TEXTURE0);
                fbTexture->bind();
            }

            for (int i = 0; i < extra_textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i + 1);
                extra_textures.at(i)->bind();
            }

            glBindVertexArray(fbVao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        virtual void drawToDefault(glm::vec2 offset = glm::vec2(0), glm::vec2 window_size = glm::vec2(0), bool clear = true, float zoom = 1.0, std::vector<Bloom::Texture2D*> extra_textures = {}, float depth = 0.99) {
            bindDefaultFramebuffer(clear, window_size.x, window_size.y, offset, zoom); 
            draw(depth, extra_textures);

        }
        virtual void drawToDefault(glm::vec2 offset = glm::vec2(0), glm::vec2 viewport_size = glm::vec2(0), bool clear = true, float zoom = 1.0, float depth = 0.99) {
            drawToDefault(offset, viewport_size, clear, zoom, {}, depth);
        }

        uint16_t getWidth() { return size.x; };
        uint16_t getHeight() { return size.y; };
        glm::ivec2 getSize() { return size; };
        uint16_t getVBO() { return fbVbo; };
        uint16_t getVBA() { return fbVao; };
        uint16_t getFBO() { return fbo; };
        Texture2D* getTexture() { return fbTexture; };
        void SetTexture(Texture2D* texture) {
            fbTexture = texture;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture->GetID(), 0);
            size.x = fbTexture->GetSize().x;
            size.y = fbTexture->GetSize().y;
        };
        uint16_t getRBO() { return rbo; };

        Bloom::Shader* GetShader() { return &fbShader; };

        void setShader(const char* fragment_source) {
            fbShader = Shader("Application/res/shaders/framebuffer/framebuffer_vertex.glsl", fragment_source);
        };
        void setTextureUniform(const char* uniform, int id = 0) {
            fbShader.use(); fbShader.uniformInt(fbShader.getUniformId(uniform), id);
        }


        void SetLocationOffset(glm::vec2 offset) {
            this->offset = offset;
        }

        void AddTextureComponent(Bloom::Texture2D* texture) {
            use(false);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_attachments, GL_TEXTURE_2D, texture->GetID(), 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            if (fbTexture == nullptr)
                fbTexture = texture;
            
            attached_textures.push_back(texture);

            attached_buffers.push_back(GL_COLOR_ATTACHMENT0 + color_attachments);
            color_attachments++;
        }
    private:
        void initFramebuffer() {
            // FRAMEBUFFER CREATION
            glGenVertexArrays(1, &fbVao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
            glBindVertexArray(fbVao);

            float fbVerts[] = {
                // POSITION    Texture
                -1, -1,         0,0,
                1, -1,          1,0,
                1, 1,           1,1,
                -1,1,           0,1,
                -1, -1,         0,0,
                1, 1,           1,1,
            };

            glGenBuffers(1, &fbVbo); // Set vbo id
            glBindBuffer(GL_ARRAY_BUFFER, fbVbo); // bind the vbo as an array buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(fbVerts), &fbVerts, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), ((void*)(0 * sizeof(float)))); // Position
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), ((void*)(2 * sizeof(float))));
            glEnableVertexAttribArray(1);

            glGenFramebuffers(1, &fbo);

            fbShader.use();
            this->offset_uid = fbShader.getUniformId("offset");
            this->depth_uid = fbShader.getUniformId("custom_depth");
        }
        void AddDepthStencilComponent() {
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        glm::ivec2 size;

        uint32_t fbVao;
        uint32_t fbVbo;

        uint32_t fbo;
        Texture2D* fbTexture = nullptr;
        uint32_t rbo;

        Shader fbShader = Shader("Application/res/shaders/framebuffer/framebuffer_vertex.glsl", "Application/res/shaders/framebuffer/framebuffer_fragment_alpha.glsl");

        glm::vec2 offset = glm::vec2(0);
        uint8_t offset_uid;
        uint8_t depth_uid;

        int color_attachments = 0;

        std::vector<Texture2D*> attached_textures;
        std::vector<GLenum> attached_buffers;
    };
}