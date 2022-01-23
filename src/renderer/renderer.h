#ifndef RENDERER
#define RENDERER

#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include <iostream>
#include <math.h>
#include <string>
#include <chrono> // Time

#include "../modules.h"
#include "../../Game/engine.h"

namespace Bloom{
    class Renderer{
        public:
        Renderer(glm::vec2 window_size,glm::vec2 pixel_size,void (*mainLoop)(), void (*inputLoop)(), void (*uiLoop)(),std::string windowName = "Bloom",bool resizeable = false,bool borderless = false,bool maximised = false, bool transparent = false);
        void Update();


        float GetTime();
        int GetFPS();

        bool GetLocked();
        void SetLocked(bool locked);

        glm::vec2 GetWindowSize();
        glm::vec2 GetPixelSize();

        GLFWwindow* GetWindow();

        glm::mat4 getProjectionMatrix();
        glm::mat4 getPixelProjectionMatrix();
        glm::mat4 getViewMatrix();

        private:
        GLFWwindow* window;

        float time = 0;
        bool locked = false;
        int fps;

        glm::vec2 windowSize;
        glm::vec2 pixelSize;

        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 pixelProjection;

        Bloom::Framebuffer* fb_pixels;

        void (*InputLoop)() = nullptr;
        void (*MainLoop)() = nullptr;
        void (*UILoop)() = nullptr;
    };
}

#endif