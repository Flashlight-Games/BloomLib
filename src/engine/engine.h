#ifndef ENGINE
#define ENGINE

#include "../src/dependencies.h"

namespace Game{
    class Engine{
        public:
            Engine(bool init = false){if(init){Init();};};
            int Run() { return Init(); };
            
            glm::mat4 GetWindowProjectionMatrix() { return window_projection_matrix; };
            glm::mat4 GetPixelProjectionMatrix() { return pixel_projection_matrix; };
            glm::mat4 GetViewMatrix() { return view_matrix; };

            glm::vec2 GetWindowSize() { return glm::vec2(windowWidth, windowHeight); };
            glm::vec2 GetPixelWindowSize() { return glm::vec2(pixel_windowWidth, pixel_windowHeight); };

            int GetFPS() { return fps; };
            float GetTime() { return time; };
        private:
            // EVENTS
            int Init();
            int Prep_Begin();
            virtual void Begin();

            // LOOPS
            int Prep_GameLoop();
            virtual void GameLoop();
            int Prep_UILoop();
            virtual void UILoop();

            // BACK
            void InputLoop();
            int EndLoop();

            glm::mat4 window_projection_matrix;                                 // PROJECTION MATRIX FOR WINDOW
            glm::mat4 pixel_projection_matrix;                                  // PROJECTION MATRIX FOR PIXELS
            glm::mat4 view_matrix;                                              // CAMERA VIEW MATRIX

            uint16_t windowWidth = 800; uint16_t windowHeight = 600;            // WINDOW SIZE
            uint16_t pixel_windowWidth = 64; uint16_t pixel_windowHeight = 48;  // PIXEL SIZE


            Bloom::Framebuffer* fb_pixels;                                      // PIXEL FRAMEBUFFER
            GLFWwindow* window;                                                 // APPLICATION WINDOW
            int fps = 0;                                                        // FRAMES PER SECOND
            float time = 0;                                                     // APP TIME
            bool isLocked = false;                                              // TIME IS LOCKED
    };

    void ResizeLoop(GLFWwindow* window, int width, int height); // GLFW RESIZING
}

#endif