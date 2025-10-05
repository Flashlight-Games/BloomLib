#include <glad/glad.h>
#include <GLM/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "STBI/stb_image.h"

export module Bloom.Textures;

import std;

export namespace Bloom {
    class Texture2D {
    public:
        Texture2D(const void* data, glm::ivec2 size, bool linear_scaling = true, int format = GL_RGBA, int internal_format = GL_RGBA16F, int type = GL_UNSIGNED_BYTE) : size(size) {
            Initialize(data, size, linear_scaling, format, internal_format, type);
        }
        Texture2D(const char* path, bool linear_scaling = true) : path(path) {
            int channels;
            const char* source = path;
            unsigned char* image = stbi_load(source, &size.x, &size.y, &channels, 0);

            int format = 0;
            int internal_format = 0;

            switch (channels) {
            case 4:
                format = GL_RGBA; internal_format = GL_RGBA16F;
                break;
            case 3:
                format = GL_RGB; internal_format = GL_RGB16F;
                break;
            case 1:
                format = GL_RED; internal_format = GL_R16F;
                break;
            default:
                break;
            }

            Initialize(image, size, linear_scaling, format, internal_format, GL_UNSIGNED_BYTE);
            stbi_image_free(image);
        }
        Texture2D(const Texture2D& copy) : path(copy.path), size(copy.size), id(copy.id) { }

        ~Texture2D() {
            glDeleteTextures(1, &this->id);
        }

        void bind() {
            glBindTexture(GL_TEXTURE_2D, id);
        }

        const glm::vec2 GetSize() { return size; }
        GLuint GetID() { return this->id; }
        auto GetPath() { return path; }
    private:
        void Initialize(const void* data, glm::ivec2 size, bool linear_scaling, int format, int internal_format, int type) {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size[0], size[1], 0, format, type, data);

            if (linear_scaling) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }

        const char* path;
        glm::ivec2 size;
        GLuint id = -1;
    };
}