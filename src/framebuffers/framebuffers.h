#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include "../shaders/shaders.h"

class Framebuffer{
    public:
    uint32_t fbVao;
    uint32_t fbVbo;
    Shader fbShader = Shader("src/shaders/framebuffer/framebuffer_vertex.glsl","src/shaders/framebuffer/framebuffer_fragment.glsl");
    uint32_t fbo;
    uint32_t fbTexture;
    uint32_t rbo;

    Framebuffer(int width,int height);
    void use(bool clear);
    void draw();

    void setShader(const char* fragment_source);
};

void bindDefaultFramebuffer(bool clear);