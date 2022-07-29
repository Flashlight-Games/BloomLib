#ifndef ENGINE
#define ENGINE

#include "dependencies.h"
#include "utils/utils.h"

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
                if (SteamTools::SteamManager::Init()) {
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
                }

                return false;
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
            void DrawPixelsToDefault(bool clear = true) { (*fb_pixels).drawToDefault(glm::vec2(windowWidth, windowHeight),clear); bindDefaultFramebuffer(false, windowWidth, windowHeight); };
            void DrawPixels() { (*fb_pixels).draw(); };
            void SetCameraLocation(glm::vec2 location){ view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(location.x, location.y, -1)); }

            double GetDeltaTime() { return frame_time * delta_multiplier; };
            double GetGameSpeed() { return GetDeltaTime() / 16.67; };

            int PlaySound(SoLoud::Wav* sample) { return soloud.play(*sample); };
            int PlaySound(SoLoud::Wav* sample, float pitch) { int id = soloud.play(*sample); soloud.setRelativePlaySpeed(id, pitch); return id; };
            void StopSound(int id) { soloud.stop(id); };

            int PlaySoundBackground(SoLoud::Wav* sample,float volume, bool looping) { int id = soloud.playBackground(*sample,volume); soloud.setLooping(id,looping); return id; };

            void FadeVolume(int id,int target, int duration) { soloud.fadeVolume(id,target ,duration); };
            SoLoud::Soloud* GetSound() { return &soloud; };

            void SetGameSpeedMultiplier(float multiplier) { delta_multiplier = multiplier; };
            float GetGameSpeedMultiplier() { return delta_multiplier; };

            auto GetSteamManager() { return steam_manager; };
            void DrawDebugPoint(glm::vec2 position, int size = 1, glm::vec3 color = glm::vec3(1, 1, 1)) { if (Utils::isDevelopmentConfiguration()) { debug_sprite->SetLocation(position); debug_sprite->SetRotation(0); debug_sprite->SetScale(glm::vec2(size / 16.0)); debug_sprite->GetShader()->use(); debug_sprite->GetShader()->uniformVec4(debug_sprite->GetShader()->getUniformId("uColor"), glm::vec4(color, 1)); debug_sprite->Draw(GetViewMatrix()); } };
            void DrawDebugLine(glm::vec2 positionA, glm::vec2 positionB, int thickness = 1, glm::vec3 color = glm::vec3(1, 1, 1)) {
                if (Utils::isDevelopmentConfiguration()) {
                    float height = (std::sqrt(std::pow(positionA.x - positionB.x, 2) + std::pow(positionA.y - positionB.y, 2)));
                    
                    float angle = glm::degrees(atan2(positionA.y - positionB.y, positionA.x - positionB.x)) - 90;
                    glm::vec2 position = (positionA + positionB) / glm::vec2(2);
                    debug_sprite->SetLocation(position); debug_sprite->SetRotation((int)(angle + 180) % 180); debug_sprite->SetScale(glm::vec2(thickness / 16.0, height / 16.0)); debug_sprite->GetShader()->use(); debug_sprite->GetShader()->uniformVec4(debug_sprite->GetShader()->getUniformId("uColor"), glm::vec4(color, 1)); debug_sprite->Draw(GetViewMatrix());
                }
            }
            std::chrono::steady_clock::time_point GetHighResTime() { return std::chrono::high_resolution_clock::now(); };

            std::thread* update_thread;
            std::chrono::steady_clock::time_point last_update_time;
            std::chrono::steady_clock::time_point current_update_time;
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
            float fps = 0;                                                      // FRAMES PER SECOND
            float game_time = 0;                                                // APP TIME
            bool isLocked = false;                                              // TIME IS LOCKED

            glm::vec2 cursor_position = glm::vec2(0, 0);                        // CURSOR POSITION

            std::chrono::steady_clock::time_point last_time = GetHighResTime(); // TIME AT LAST FRAME
            double frame_time = 0.0;                                            // DELTA TIME BETWEEN FRAMES IN MS
            float delta_multiplier = 1.0;                                       // DELTA SPEED MULTIPLIER

            configuration engine_config;                                        // ENGINE CONFIGURATION
            SteamTools::SteamManager* steam_manager;                            // STEAM MANAGER

            Bloom::Sprite* debug_sprite;
    };

    void ResizeLoop(GLFWwindow* window, int width, int height); // GLFW RESIZING
}

#endif