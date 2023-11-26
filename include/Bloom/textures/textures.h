#ifndef BLOOM_TEXTURES
#define BLOOM_TEXTURES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "STBI/stb_image.h"

#include "dearImGui/imgui.h"
#include "dearImGui/imgui_impl_glfw.h"
#include "dearImGui/imgui_impl_opengl3.h"

#include <vector>
#include <iostream>

namespace Bloom{
    class Texture2D{
        public:
            Texture2D(const char* path,bool linear_scaling = true);
            Texture2D(const void* data,glm::vec2 size,int format = GL_RGBA,bool linear_scaling = true);
            Texture2D();
            Texture2D(Texture2D& copy);
            ~Texture2D();
            void ImGui(float scale = 1) {
                ImGui::Image((void*)GetID(), ImVec2(GetSize().x * scale, GetSize().y * scale));
            };

            void bind();

            const glm::vec2 GetSize(){return size;};
            GLuint GetID(){return id;};

            auto GetPath() { return path; };
        private:
            const char* path;
            glm::vec2 size;
            GLuint id;
            int format = GL_RGBA;
    };    
}

glm::vec2 getTextureDimensions(GLuint textureID,GLuint mipLevel = 0);

#endif

// To Display in ImGui
// ImGui::Image((void*)(intptr_t)fb_pixels.getTexture().getID(), ImVec2(fb_pixels.getTexture().getSize()[0],fb_pixels.getTexture().getSize()[1]));