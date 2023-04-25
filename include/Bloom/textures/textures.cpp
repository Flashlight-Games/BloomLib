#define STB_IMAGE_IMPLEMENTATION
#include "textures.h"

using namespace Bloom;

Texture2D::Texture2D(const void* data,glm::vec2 size,int format,bool linear_scaling){
    this->size = size;
    this->format = format;

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    if(format != GL_RGBA16F)
        glTexImage2D(GL_TEXTURE_2D, 0, format, size[0],size[1], 0, format, GL_UNSIGNED_BYTE,data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size[0], size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    if(linear_scaling){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else{
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

Texture2D::Texture2D(const char* path,bool linear_scaling){
    this->path = path;

    int width, height, channels;
    const char* source = path;
    unsigned char* image = stbi_load(source,&width,&height,&channels,0);

    this->size[0] = width;
    this->size[1] = height;

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    if (channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size[0], size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    else if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size[0], size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (channels == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size[0], size[1], 0, GL_RED, GL_UNSIGNED_BYTE, image);
    else if (stbi_failure_reason())
        throw std::runtime_error(std::string(stbi_failure_reason()) + " \""+std::string(source)+"\"");

    if(linear_scaling){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else{
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    stbi_image_free(image);
}

void Texture2D::bind(){
    glBindTexture(GL_TEXTURE_2D,id);
}

glm::vec2 getTextureDimensions(GLuint textureID, GLuint mipLevel){
    int w, h;
    glBindTexture(GL_TEXTURE_2D,textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &h);
    return glm::vec2(w,h);
}