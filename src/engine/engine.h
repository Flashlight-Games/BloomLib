#ifndef ENGINE
#define ENGINE

#include "../src/dependencies.h"

namespace Game{
    struct configuration {
        glm::vec2 resolution_window_size = glm::vec2(420, 420);
        glm::vec2 resolution_pixels_size = glm::vec2(64, 64);

        bool glfw_resizeable = false;
        bool glfw_decorated = true;
        bool glfw_topmost = false;
        bool glfw_maximized = false;
        bool glfw_center_cursor = false;
        bool glfw_transparent = false;

        bool rendering_face_culling = false;
        bool rendering_anti_aliasing = false;
        bool rendering_enable_vsync = true;

        std::string window_name = "Bloom";
    };

    class Engine{
        public:
            Engine(configuration config, bool init = false) { this->engine_config = config; if (init) { Run(); }; };
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
            float GetTime() { return game_time; };
            GLFWwindow* GetWindow() { return window; };
            glm::vec2 GetCursorPosition() { return cursor_position; };
            glm::vec2 GetScale() { return GetWindowSize() / GetPixelWindowSize(); };

            void EnablePixels(bool clear = true) { (*fb_pixels).use(clear); };
            void DrawPixelsToDefault() { (*fb_pixels).drawToDefault(windowWidth, windowHeight); bindDefaultFramebuffer(false, windowWidth, windowHeight); };
            void DrawPixels() { (*fb_pixels).draw(); };
            void SetCameraLocation(glm::vec2 location){ view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(location.x, location.y, -1)); }

            double GetFrameTime() { return frame_time; };
            double GetGameSpeed() { return GetFrameTime() / 1000.0; };

            int PlaySound(SoLoud::Wav* sample) { return soloud.play(*sample); };
            int PlaySound(SoLoud::Wav* sample, float pitch) { int id = soloud.play(*sample); soloud.setRelativePlaySpeed(id, pitch); return id; };
            void StopSound(int id) { soloud.stop(id); };

            int PlaySoundBackground(SoLoud::Wav* sample,float volume, bool looping) { int id = soloud.playBackground(*sample,volume); soloud.setLooping(id,looping); return id; };

            void FadeVolume(int id,int target, int duration) { soloud.fadeVolume(id,target ,duration); };
            SoLoud::Soloud* GetSound() { return &soloud; };

            //virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)

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

            SoLoud::Soloud soloud;                                              // AUDIO MANAGER


            Bloom::Framebuffer* fb_pixels;                                      // PIXEL FRAMEBUFFER
            GLFWwindow* window;                                                 // APPLICATION WINDOW
            int fps = 0;                                                        // FRAMES PER SECOND
            float game_time = 0;                                                // APP TIME
            bool isLocked = false;                                              // TIME IS LOCKED

            glm::vec2 cursor_position = glm::vec2(0, 0);                        // CURSOR POSITION

            double last_time = 0;                                               // TIME AT LAST FRAME
            double frame_time = 0;                                              // DELTA TIME BETWEEN FRAMES

            configuration engine_config;
    };

    void ResizeLoop(GLFWwindow* window, int width, int height); // GLFW RESIZING
}

namespace Utils {
    template <typename T> T Lerp(T A, T B, float percentage) {
        return A + (B - A) * percentage;
    };

    float Distance2D(glm::vec2 A, glm::vec2 B);

    glm::vec2 PointAtAngleAndDistance(float angle, float distance);

    void SyncSleep(Game::Engine* app, int target_fps = 60);
    void SleepMS(int ms);

    int RandInt(int min, int max);

    float RandFloat(float min, float max);
}

#endif