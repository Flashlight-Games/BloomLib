#include "STBI/stb_image.h"
#include "Bloom/dependencies.h"
#include <csignal>

export module Bloom.Engine;

import Bloom.Utils;
import Bloom.Collision;
import Bloom.Textures;
import Bloom.Sprite;
import Bloom.Text;
import Bloom.Framebuffers;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

void ResizeLoop(GLFWwindow* window, int width, int height);

export namespace Game {
    struct configuration {
        glm::vec2 resolution_window_size = glm::vec2(420, 420);
        glm::vec2 resolution_pixels_size = glm::vec2(64, 64);

        bool glfw_resizeable = false;
        bool glfw_decorated = true;
        bool glfw_full_screen = false;
        bool glfw_topmost = false;
        bool glfw_maximized = false;
        bool glfw_transparent = false;
        bool glfw_show_cursor = true;
        bool glfw_use_depth = true;

        bool rendering_face_culling = false;
        bool rendering_anti_aliasing = false;
        bool rendering_enable_vsync = true;

        bool feature_enable_steam = true;
        bool feature_enable_parallel_loop = true;

        std::string window_name = "Bloom";
        const char* icon_path = "Application/res/textures/BloomLogo.png";
    };

    class Engine {
    public:
        Engine(configuration config) : engine_config(config) { 
            //feature_enable_steam
            bool should_init_engine = !engine_config.feature_enable_steam;
            if (engine_config.feature_enable_steam)
                should_init_engine = SteamTools::SteamManager::Init();
            if (!should_init_engine)
                throw std::runtime_error("Failed to Initialize Steam!");

            Init();
        };
        void Run() {
            Prep_Begin();

            while (running) {
                Prep_GameLoop();
                Prep_UILoop();
                EndLoop();
            }
            
            CloseGame();
        };

        glm::mat4* GetWindowProjectionMatrix() { return &window_projection_matrix; };
        glm::mat4* GetPixelProjectionMatrix() { return &pixel_projection_matrix; };
        glm::mat4* GetViewMatrix() { return &view_matrix; };

        glm::vec2 GetWindowSize() { return glm::vec2(windowWidth, windowHeight); };
        glm::vec2 GetPixelWindowSize() { return glm::vec2(pixel_windowWidth, pixel_windowHeight); };
        glm::vec2 GetRenderSize() { return renderSize; };
        glm::vec4 GetViewportParams() { return viewportParams; };


        int GetFPS() const { return fps; };
        int GetUPS() { return ups; };
        int GetAverageFPS() { return average_fps; };
        float GetTime() { return game_time * delta_multiplier; };
        GLFWwindow* GetWindow() { return window; };
        void OverrideRenderSize(glm::vec2 size) { this->renderSize = size; }
        void SetWindowSize(glm::vec2 size) { this->windowWidth = size.x; this->windowHeight = size.y; }
        void SetViewportParams(glm::vec4 params) { this->viewportParams = params; }
        glm::vec2 GetCursorPosition() { return cursor_position; };
        glm::vec2 GetCursorWindowPosition() { return this->cursor_position_window; };
        glm::vec2 GetScale() { return GetWindowSize() / GetPixelWindowSize(); };

        void SetMouseOffset(glm::vec2 offset) { this->mouseOffset = offset; }

        void EnablePixels(bool clear = true) { (*fb_pixels).use(clear); };
        void EnablePixels(int clear_range) { (*fb_pixels).use(clear_range); };
        void DrawPixelsToDefault(bool clear = true, float depth = 0.99) { (*fb_pixels).drawToDefault(glm::vec2(viewportParams) + (glm::vec2(windowWidth / pixel_windowWidth, windowHeight / pixel_windowHeight) / -2.0f), glm::vec2(viewportParams.b, viewportParams.a), clear, zoom, depth); };
        void DrawPixels(float depth = 0.99) { (*fb_pixels).draw(depth); };
        void SetCameraLocation(glm::vec2 location) {
            location = -location;
            view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(location, -1));
            fb_pixels->SetLocationOffset((location - glm::round(location)) / GetPixelWindowSize() * 2.0f);
        }
        auto GetPixelBuffer() { return this->fb_pixels; };

        double GetDeltaTime() { return frame_time * delta_multiplier; };
        double GetDeltaTimeMS() { return frame_time * delta_multiplier / 1000.0f; };
        double GetGameSpeed() { return GetDeltaTime() / (1000.0 / 60.0); };

        int PlaySound(SoLoud::Wav* sample) { return soloud->play(*sample); };
        int PlaySound(SoLoud::Wav* sample, float pitch) { int id = soloud->play(*sample); soloud->setRelativePlaySpeed(id, pitch); return id; };
        void StopSound(int id) { soloud->stop(id); };

        int PlaySoundBackground(SoLoud::Wav* sample, float volume, bool looping) { int id = soloud->playBackground(*sample, volume); soloud->setLooping(id, looping); return id; };

        void FadeVolume(int id, int target, int duration) { soloud->fadeVolume(id, target, duration); };
        SoLoud::Soloud* GetSound() { return soloud; };

        void SetGameSpeedMultiplier(float multiplier) { delta_multiplier = multiplier; };
        float GetGameSpeedMultiplier() { return delta_multiplier; };

        auto GetSteamManager() { return steam_manager; };
        void DrawDebugPoint(glm::vec2 position, int size = 1, glm::vec3 color = glm::vec3(1, 1, 1), float depth = 0.f) {
#ifdef DEBUG
            debug_sprite->SetLocation(position); debug_sprite->SetRotation(0); debug_sprite->SetScale(glm::vec2(size / 16.0)); debug_sprite->GetShader()->use(); debug_sprite->GetShader()->uniformVec4(debug_sprite->GetShader()->getUniformId("uColor"), glm::vec4(color, 1)); debug_sprite->Draw(depth);
#endif
        };
        void DrawDebugPoint(glm::vec2 position, glm::vec2 size = glm::vec2(1), glm::vec3 color = glm::vec3(1, 1, 1), float depth = 0.f) {
#ifdef DEBUG
            debug_sprite->SetLocation(position); debug_sprite->SetRotation(0); debug_sprite->SetScale(size / 16.0f); debug_sprite->GetShader()->use(); debug_sprite->GetShader()->uniformVec4(debug_sprite->GetShader()->getUniformId("uColor"), glm::vec4(color, 1)); debug_sprite->Draw(depth);
#endif
        };
        void DrawDebugPoint(glm::vec2 position, glm::vec2 size = glm::vec2(1), glm::vec4 color = glm::vec4(1), float depth = 0.f) {
#ifdef DEBUG
            debug_sprite->SetLocation(position); debug_sprite->SetRotation(0); debug_sprite->SetScale(size / 16.0f); debug_sprite->GetShader()->use(); debug_sprite->GetShader()->uniformVec4(debug_sprite->GetShader()->getUniformId("uColor"), color); debug_sprite->Draw(depth);
#endif
        };
        void DrawDebugLine(glm::vec2 positionA, glm::vec2 positionB, float thickness = 1, glm::vec3 color = glm::vec3(1, 1, 1), float depth = 0.f) {
#ifdef DEBUG
            float height = (std::sqrt(std::pow(positionA.x - positionB.x, 2) + std::pow(positionA.y - positionB.y, 2)));

            float angle = glm::degrees(atan2(positionA.y - positionB.y, positionA.x - positionB.x)) - 90;
            glm::vec2 position = (positionA + positionB) / glm::vec2(2);
            debug_sprite->SetLocation(position); debug_sprite->SetRotation(angle + 180); debug_sprite->SetScale(glm::vec2(thickness / 16.0, height / 16.0)); debug_sprite->GetShader()->use(); debug_sprite->GetShader()->uniformVec4(debug_sprite->GetShader()->getUniformId("uColor"), glm::vec4(color, 1)); debug_sprite->Draw(depth);
#endif // DEBUG
        }
        void DrawDebugCollision(Bloom::collisionBox* collision, float thickness = 1, glm::vec3 color = glm::vec3(1, 1, 1), float depth = 0.f) {
#ifdef DEBUG
            glm::vec2 location = collision->getLocation();
            glm::vec2 rotation_axis = collision->getLocation() + (collision->getOrigin() - glm::vec2(0.5)) * collision->getScale();
            glm::vec2 offset = -(collision->getOrigin() - glm::vec2(0.5)) * collision->getScale();
            DrawDebugLine(offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(0.5, 0.5), collision->getRotation(), rotation_axis), offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(0.5, -0.5), collision->getRotation(), rotation_axis), thickness, color, depth);
            DrawDebugLine(offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(0.5, -0.5), collision->getRotation(), rotation_axis), offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(-0.5, -0.5), collision->getRotation(), rotation_axis), thickness, color, depth);
            DrawDebugLine(offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(-0.5, -0.5), collision->getRotation(), rotation_axis), offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(-0.5, 0.5), collision->getRotation(), rotation_axis), thickness, color, depth);
            DrawDebugLine(offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(-0.5, 0.5), collision->getRotation(), rotation_axis), offset + Utils::rotateVector(location + collision->getScale() * glm::vec2(0.5, 0.5), collision->getRotation(), rotation_axis), thickness, color, depth);
#endif // DEBUG
        }
        std::chrono::steady_clock::time_point GetHighResTime() { return std::chrono::high_resolution_clock::now(); };

        std::thread* update_thread;
        std::chrono::steady_clock::time_point last_update_time;
        std::chrono::steady_clock::time_point current_update_time;

        glm::vec2 GetCursorWorldPosition(glm::vec2 camera_location) {
            return (GetCursorPosition() - (GetPixelWindowSize() / glm::vec2(2)) + GetCameraWorldPosition(camera_location));
        }
        glm::vec2 GetCameraWorldPosition(glm::vec2 camera_location) {
            return (GetPixelWindowSize() / glm::vec2(2)) - camera_location;
        }

        void SetCameraZoom(float zoom) { this->zoom = zoom; };
        auto GetCameraZoom() { return this->zoom; };

        void ExitGame() {
            glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
        }

        auto GetFontAlagard() { return this->font_alagard; };
        auto GetFontRoboto() { return this->font_roboto; };
        auto GetFontPressStart() { return this->font_pressstart; };
    protected:
        virtual void CloseGame() {
            if (!running)
                return;

            running = false;

            soloud->deinit();
            SteamAPI_Shutdown();
            glfwDestroyWindow(GetWindow());
            glfwTerminate();

            delete fb_pixels;
            delete steam_manager;
            delete debug_sprite;

            std::cout << Utils::Default() << Utils::Default(true);
        }

    private:
        // EVENTS
        void Init() {
            install_global_handlers();

            glfwSetErrorCallback(error_callback);

            // CONFIGURATION
            this->windowWidth = engine_config.resolution_window_size.x;
            this->windowHeight = engine_config.resolution_window_size.y;
            this->pixel_windowWidth = engine_config.resolution_pixels_size.x;
            this->pixel_windowHeight = engine_config.resolution_pixels_size.y;
            pixel_ratio = glm::vec2(pixel_windowWidth / (float)windowWidth, pixel_windowHeight / (float)windowHeight);

            // INITIALIZATION
            glfwInit();

            srand((unsigned)time(NULL));

            glfwWindowHint(GLFW_RESIZABLE, engine_config.glfw_resizeable);
            glfwWindowHint(GLFW_DECORATED, engine_config.glfw_decorated);
            glfwWindowHint(GLFW_FLOATING, engine_config.glfw_topmost);
            glfwWindowHint(GLFW_MAXIMIZED, engine_config.glfw_maximized);
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, engine_config.glfw_transparent);
            glfwWindowHint(GLFW_DEPTH_BITS, 24);
            glfwWindowHint(GLFW_SAMPLES, 4);

            if (engine_config.glfw_maximized && engine_config.glfw_full_screen) {
                const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

                windowWidth = mode->width;
                windowHeight = mode->height;
            }

            GLFWmonitor* target_monitor = (engine_config.glfw_full_screen ? glfwGetPrimaryMonitor() : NULL);
            window = glfwCreateWindow(windowWidth, windowHeight, engine_config.window_name.c_str(), target_monitor, NULL);
            if (window == NULL) {
                throw std::runtime_error("Failed to create GLFW window");
            }
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            windowWidth = width; windowHeight = height;
            OverrideRenderSize(glm::vec2(windowWidth, windowHeight));

            glfwSetFramebufferSizeCallback(window, ResizeLoop);
            glfwMakeContextCurrent(window);

            if (!engine_config.glfw_show_cursor) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // HIDE CURSOR
                if (glfwRawMouseMotionSupported())
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                auto center = GetWindowSize() / 2.0f;
                glfwSetCursorPos(GetWindow(), center.x, center.y);
            }

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {// GLAD INIT, GL Context init 
                throw std::runtime_error("Failed to initialize GLAD");
            }

            stbi_set_flip_vertically_on_load(true);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if (engine_config.rendering_face_culling)
                glEnable(GL_CULL_FACE);            // Disable Double Sided Polygons
            else
                glDisable(GL_CULL_FACE);           // Enable Double Sided Polygons

            if (engine_config.rendering_anti_aliasing) {
                glEnable(GL_MULTISAMPLE);          // ENABLE ANTI-ALIASING
            } else
                glDisable(GL_MULTISAMPLE);          // Disable ANTI-ALIASING

            if (engine_config.glfw_use_depth)
                glEnable(GL_DEPTH_TEST);            // ENABLE DEPTH TESTING
            else
                glDisable(GL_DEPTH_TEST);           // DISABLE DEPTH TESTING

            glDepthFunc(GL_LESS);
            glClearDepth(1.0);
            glDepthMask(GL_TRUE);

            glfwSwapInterval(engine_config.rendering_enable_vsync);    // Enble VSYNC

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 430");

            // CREATE PROJ MATRICES
            pixel_projection_matrix = glm::ortho(0.0f, (float)pixel_windowWidth, 0.0f, (float)pixel_windowHeight, (float)-1000, (float)1000); // ORTHO PROJECTION, window size you want to render at
            window_projection_matrix = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, (float)-1000, (float)1000); // ORTHO PROJECTION, window size you want to render at
            view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));

            fb_pixels = new Bloom::Framebuffer(GetPixelWindowSize(), false);

            soloud = new SoLoud::Soloud();
            soloud->init();

            glfwSetWindowUserPointer(GetWindow(), this);

            GLFWimage images[1];
            images[0].pixels = stbi_load(engine_config.icon_path, &images[0].width, &images[0].height, 0, 4); //rgba channels 
            glfwSetWindowIcon(window, 1, images);
            stbi_image_free(images[0].pixels);

            steam_manager = new SteamTools::SteamManager();
            debug_sprite = new Bloom::Sprite("Application/res/textures/rect16.png", 1, GetViewMatrix(), GetWindowProjectionMatrix());

            this->font_alagard = new Bloom::Text("Application/res/fonts/Alagard/alagard.ttf", GetViewMatrix(), GetWindowProjectionMatrix(), -0.5f, 256);
            this->font_roboto = new Bloom::Text("Application/res/fonts/Roboto/Roboto-Black.ttf", GetViewMatrix(), GetWindowProjectionMatrix(), -0.5f, 256);
            this->font_pressstart = new Bloom::Text("Application/res/fonts/PressStart/PressStart.ttf", GetViewMatrix(), GetWindowProjectionMatrix(), -0.5f, 256);

            glViewport(0, 0, windowWidth, windowHeight);
            viewportParams = glm::vec4(0, 0, windowWidth, windowHeight);
            glClearColor(0.1, 0.1, 0.1, 1.0); // THIS IS RESET BY THE SPLASH SCREEN
            
        }
        void Prep_Begin() {
            if (engine_config.feature_enable_parallel_loop)
                update_thread = new std::thread(&Engine::UpdateLoop, this);
            Begin();
        }

        virtual void Begin() {
            std::string errmsg = "Please define application method 'Begin'";
            throw std::runtime_error(errmsg);
        }

        // LOOPS
        void Prep_GameLoop() {
            if (!window_init_ready) {
                window_init_ready = true;
                glfwSetTime(0.0);
                return;
            }

            // PREPARE LOOP
            SteamTools::SteamManager::ProcessCallbacks();
            double delta_time = GetDeltaTime();
            if (delta_time > 0) {
                fps = 1000 / delta_time;
            }

            if (glfwGetTime() >= last_fps_time + 1) {
                this->last_fps_time = glfwGetTime();
                this->average_fps = (fps + prev_fps) / 2.0;
                this->prev_fps = fps;
            }

            glfwPollEvents();
            Prep_InputLoop();
            if (!isLocked)
                game_time = glfwGetTime();

            frame_time = std::chrono::duration<double>(GetHighResTime() - last_time).count() * 1000;
            last_time = GetHighResTime();

            double temp_cursor_position_x;
            double temp_cursor_position_y;
            glfwGetCursorPos(window, &temp_cursor_position_x, &temp_cursor_position_y);
            cursor_position = glm::vec2(temp_cursor_position_x, renderSize.y - temp_cursor_position_y) - mouseOffset;
            if (!engine_config.glfw_show_cursor) {
                cursor_position = glm::clamp(cursor_position, glm::vec2(0), renderSize);
            }
            cursor_position_window = cursor_position / renderSize * engine_config.resolution_window_size;

            // BINDING AND CLEARING PIXEL FRAMEBUFFER
            bindDefaultFramebuffer(true);

            if (!loaded) {
                glClearColor(0.0, 0.0, 0.0, 0.0);
                loaded = true;
            }
            else {
                return GameLoop(); // ACTUAL GAME CODE
            }
        }
        virtual void GameLoop() {
            std::string errmsg = "Please define application method 'GameLoop'";
            throw std::runtime_error(errmsg);
        }

        void Prep_UILoop() {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGuiIO& io = ImGui::GetIO();
            ImGui::NewFrame();
            if (loaded) {
                return UILoop();
            }
        }
        virtual void UILoop() {
            std::string errmsg = "Please define application method 'UILoop'";
            throw std::runtime_error(errmsg);
        }
        void Prep_InputLoop() {
            return InputLoop();
        }

        virtual void InputLoop() {
            /*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);*/
        }
        // BACK
        void EndLoop() {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Rendering pipeline end
            glfwSwapBuffers(window); // Render buffer to screen

            if (glfwWindowShouldClose(GetWindow()) && running) { // END IF SHOULD CLOSE
                return CloseGame();
            }
        }

        void UpdateLoop() {
            install_global_handlers();

            using namespace std::chrono;
            using clock = steady_clock;
            using duration = duration<double>;

            constexpr double FIXED_TIMESTEP = 1.0 / 480.0;
            constexpr auto MAX_FRAME_TIME = duration(1.0 / 20.0);

            auto lastUpdateTime = clock::now();
            double accumulator = 0.0;
            int frameCount = 0;
            auto upsTimer = clock::now();

            while (!glfwWindowShouldClose(GetWindow())) {
                auto currentTime = clock::now();
                auto frameTime = duration_cast<duration>(currentTime - lastUpdateTime);
                lastUpdateTime = currentTime;

                // Cap frame time
                frameTime = std::min(frameTime, MAX_FRAME_TIME);
                accumulator += frameTime.count();

                // Fixed timestep for physics/game logic
                while (accumulator >= FIXED_TIMESTEP) {
                    Update(static_cast<float>(FIXED_TIMESTEP));
                    accumulator -= FIXED_TIMESTEP;
                    frameCount++;
                }

                // Update UPS every second
                auto upsDuration = duration_cast<duration>(currentTime - upsTimer);
                if (upsDuration.count() >= 1.0) {
                    this->ups = frameCount / static_cast<int>(upsDuration.count());
                    frameCount = 0;
                    upsTimer = currentTime;
                }

                // Adaptive sleep
                constexpr auto MIN_SLEEP_TIME = duration(0.001); // 1ms
                auto targetTime = currentTime + duration(FIXED_TIMESTEP);
                auto sleepTime = targetTime - clock::now();

                if (sleepTime > MIN_SLEEP_TIME) {
                    std::this_thread::sleep_for(sleepTime - MIN_SLEEP_TIME);
                }

                // Yield remaining time
                std::this_thread::yield();
            }

            update_thread->detach();
            delete update_thread;
        }

        virtual void Update(float deltaTime) {};

        void install_global_handlers();

        glm::mat4 window_projection_matrix;                                 // PROJECTION MATRIX FOR WINDOW
        glm::mat4 pixel_projection_matrix;                                  // PROJECTION MATRIX FOR PIXELS
        glm::mat4 view_matrix;                                              // CAMERA VIEW MATRIX

        uint16_t windowWidth = 960; uint16_t windowHeight = 960;            // WINDOW SIZE (IN RENDER PIXELS)
        uint16_t pixel_windowWidth = 64 * 3; uint16_t pixel_windowHeight = 64 * 3;  // PIXEL SIZE
        glm::vec2 pixel_ratio = glm::vec2(pixel_windowWidth / (float)windowWidth, pixel_windowHeight / (float)windowHeight);
        glm::vec2 renderSize = glm::vec2(windowWidth, windowHeight);        // ACTUAL WINDOW SIZE
        glm::vec4 viewportParams;                                           // LAST USED VIEWPORT PARAMS
        
        glm::vec2 mouseOffset = glm::vec2(0);

        SoLoud::Soloud* soloud;                                             // AUDIO MANAGER


        Bloom::Framebuffer* fb_pixels;                                      // PIXEL FRAMEBUFFER
        GLFWwindow* window;                                                 // APPLICATION WINDOW
        float fps = 0;                                                      // FRAMES PER SECOND
        float ups = 0;                                                      // UPDATES PER SECOND (PARALLEL LOOP)
        float prev_fps = 0;                                                 // PREVIOUS FRAMES PER SECOND
        float average_fps = 0;                                              // AVERAGE FRAMES PER SECOND
        float last_fps_time = 0;                                            // LAST TIME AVERAGE FPS WAS RECORDED
        float game_time = 0;                                                // APP TIME
        bool isLocked = false;                                              // TIME IS LOCKED
        bool window_init_ready = false;                                     // PROTECTION FOR WINDOW STARTUP TIME

        glm::vec2 cursor_position_window = glm::vec2(0, 0);                 // CURSOR POSITION
        glm::vec2 cursor_position = glm::vec2(0, 0);                        // CURSOR POSITION IN PIXEL

        std::chrono::steady_clock::time_point last_time = GetHighResTime(); // TIME AT LAST FRAME
        double frame_time = 0.0;                                            // DELTA TIME BETWEEN FRAMES IN MS
        float delta_multiplier = 1.0;                                       // DELTA SPEED MULTIPLIER

        configuration engine_config;                                        // ENGINE CONFIGURATION
        SteamTools::SteamManager* steam_manager;                            // STEAM MANAGER

        Bloom::Sprite* debug_sprite;                                        // DEBUG SPRITE

        bool should_close = false;                                          // SHOULD THE GAME CLOSE

        float zoom = 1.0;                                                   // CURRENT CAMERA ZOOM

        bool running = true;                                                // IS THE GAME RUNNING
        bool loaded = false;                                                // SET TO TRUE WHEN GAME LOADS

        Bloom::Text* font_alagard;                                          // ALAGARD FONT
        Bloom::Text* font_roboto;                                           // ROBOTO FONT
        Bloom::Text* font_pressstart;                                       // PRESS STARTFONT

        float splash_screen_time = 1.0;                                     // SPLASH SCREEN DURATION
    };

    void fancyStacktrace(int skip) {
        // reset signal catchers if caught an exception, then those aren't necessary
        std::signal(SIGSEGV, SIG_DFL);
        std::signal(SIGABRT, SIG_DFL);
        
        std::cerr << "\x1b[1;39;41m";
        auto trace = std::stacktrace::current(skip);
        std::vector<int> numbers;
        std::vector<std::string> lines;
        std::vector<std::string> functionNames;
        int cutLocation = -1;
        int longest = 0;
        int maxnum = 0;

        for (int i = 0; i < trace.size(); i++) {
            auto entry = trace[i];
            auto file = entry.source_file();
            auto description = entry.description();
            if (file.contains("Dungeon") && !description.contains("install_global_handlers")) {
                auto funcStart = description.find("!");
                auto funcEnd = description.find("+");
                auto funcName = description.substr(funcStart + 1, funcEnd - funcStart - 1);
                numbers.push_back(i);
                maxnum = std::max(maxnum, i);
                functionNames.push_back(funcName);
                if (!lines.empty()) {
                    auto firstLine = lines[0];

                    for (int j = 0; j < cutLocation; j++) {
                        if (file[j] != firstLine[j])
                            cutLocation = j;
                    }
                }
                auto line = file + ":" + std::to_string(entry.source_line());
                lines.push_back(line);
                longest = std::max(longest, (int)line.length());
                if (cutLocation == -1) {
                    cutLocation = functionNames[0].length();
                }
            }
        }

        for (int i = 0; i < lines.size(); i++) {
            auto number = numbers[i];
            auto line = lines[i];
            auto functionName = functionNames[i];

            if (i == 1)
                std::cerr << "\x1b[1;39;101m";

            std::cerr << "  " << number; 
            int numdiff = maxnum / 10 - number / 10;
            for (int j = 0; j < numdiff; j++) {
                std::cout << " ";
            }
            std::cout << " - " << line.substr(cutLocation);
            for (int k = line.length(); k < longest + 2; k++) {
                std::cout << " ";
            }
            std::cout << functionName << "\n";
        }

        std::cerr << Utils::Default(true);
    }

    void signal_handler(int sig) {
        std::cerr << "\x1b[1;39;41m" << "\nCaught signal " << sig << " — dumping stack:\n";
        fancyStacktrace(2);
        std::abort();
    }

    void Engine::install_global_handlers() {
        std::set_terminate([] {
            try {
                if (auto e = std::current_exception())
                    std::rethrow_exception(e);
            }
            catch (const std::exception& e) {
                std::cerr << "\x1b[1;39;41m" << "\nUnhandled exception: " << e.what() << '\n';
            }
            fancyStacktrace(1);
            //std::abort();
            });

        // Basic POSIX / signal fallback
        std::signal(SIGSEGV, signal_handler);
        std::signal(SIGABRT, signal_handler);
        std::signal(SIGILL, signal_handler);
        std::signal(SIGFPE, signal_handler);
        std::signal(SIGTERM, signal_handler);
    }
}

void ResizeLoop(GLFWwindow* window, int width, int height) {  // GLFW RESIZING
    auto engine = static_cast<Game::Engine*>(glfwGetWindowUserPointer(window));

    float desiredRatio = 16 / 9.f;
    float minVal = glm::min(width, height);
    glm::vec2 currSize = glm::vec2(width, height);
    glm::vec2 renderArea = glm::vec2(0);
    glm::vec4 viewportParams;
    if (minVal == width) {
        viewportParams = glm::vec4(0, (height - minVal / desiredRatio) / 2.f, minVal, minVal / desiredRatio);
        renderArea = glm::vec2(minVal, minVal / desiredRatio);
    }
    else {
        viewportParams = glm::vec4((width - minVal * desiredRatio) / 2.0f, 0, minVal * desiredRatio, minVal);
        renderArea = glm::vec2(minVal * desiredRatio, minVal);
    }

    auto diff = (currSize - renderArea);
    glViewport(viewportParams.r, viewportParams.g, viewportParams.b, viewportParams.a);
    engine->SetViewportParams(viewportParams);
    engine->OverrideRenderSize(renderArea);
    engine->SetMouseOffset(diff / 2.0f);
}////////////////