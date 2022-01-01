#ifndef BLOOM_TEXTURES
#define BLOOM_TEXTURES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "STBI/stb_image.h"

namespace Bloom{
    class Texture2D{
        public:
        Texture2D(const char* path,bool linear_scaling = true);
        Texture2D(const void* data,glm::vec2 size,int format = GL_RGBA,bool linear_scaling = true);

        void bind();

        glm::vec2 getSize(){return size;};
        GLuint getID(){return id;};
        

        private:
        const char* path;
        glm::vec2 size;
        GLuint id;
    };    
}

glm::vec2 getTextureDimensions(GLuint textureID,GLuint mipLevel = 0);

#endif