#ifndef ENGINE
#define ENGINE

#include "../src/dependencies.h"

namespace Game{
    class Engine{
        public:
            Engine(bool init = false){if(init){Run();};};
            int Run() { 
                int retval = 0;
                retval = Init();
                retval = Prep_Begin();

                while (retval == 0) {
                    retval = Prep_GameLoop();
                    if (retval != 0)
                        return retval;
                    retval = Prep_UILoop();
                    if (retval != 0)
                        return retval;
                    retval = EndLoop();
                }

                return retval;
            };
            
            glm::mat4 GetWindowProjectionMatrix() { return window_projection_matrix; };
            glm::mat4 GetPixelProjectionMatrix() { return pixel_projection_matrix; };
            glm::mat4 GetViewMatrix() { return view_matrix; };

            glm::vec2 GetWindowSize() { return glm::vec2(windowWidth, windowHeight); };
            glm::vec2 GetPixelWindowSize() { return glm::vec2(pixel_windowWidth, pixel_windowHeight); };

            int GetFPS() { return fps; };
            float GetTime() { return time; };
            GLFWwindow* GetWindow() { return window; };
            glm::vec2 GetCursorPosition() { return cursor_position; };
            glm::vec2 GetScale() { return GetWindowSize() / GetPixelWindowSize(); };

            void EnablePixels() { (*fb_pixels).use(true); };
            void DrawPixels() { (*fb_pixels).drawToDefault(windowWidth, windowHeight); bindDefaultFramebuffer(false, windowWidth, windowHeight); };

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
            void Prep_InputLoop();
            virtual void InputLoop();
            // BACK
            int EndLoop();

            glm::mat4 window_projection_matrix;                                 // PROJECTION MATRIX FOR WINDOW
            glm::mat4 pixel_projection_matrix;                                  // PROJECTION MATRIX FOR PIXELS
            glm::mat4 view_matrix;                                              // CAMERA VIEW MATRIX

            uint16_t windowWidth = 960; uint16_t windowHeight = 960;            // WINDOW SIZE
            uint16_t pixel_windowWidth = 64*3; uint16_t pixel_windowHeight = 64*3;  // PIXEL SIZE


            Bloom::Framebuffer* fb_pixels;                                      // PIXEL FRAMEBUFFER
            GLFWwindow* window;                                                 // APPLICATION WINDOW
            int fps = 0;                                                        // FRAMES PER SECOND
            float time = 0;                                                     // APP TIME
            bool isLocked = false;                                              // TIME IS LOCKED

            glm::vec2 cursor_position = glm::vec2(0, 0);                        // CURSOR POSITION
    };

    void ResizeLoop(GLFWwindow* window, int width, int height); // GLFW RESIZING
}

namespace Utils {
    template <typename T> T Lerp(T A, T B, float percentage) {
        return A + (B - A) * percentage;
    }

    float Distance2D(glm::vec2 A, glm::vec2 B);
}

#endif