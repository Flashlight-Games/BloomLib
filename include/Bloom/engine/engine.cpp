#include "engine.h"

using namespace Game;
using namespace nlohmann;

// GLFW RESIZE
void Game::ResizeLoop(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

// BACKEND
void Engine::Prep_InputLoop(){
    try {
        InputLoop();
    }
    catch (const std::exception& ex) {
        std::cerr << "Encountered Bloom Exception during InputLoop : " << ex.what() << std::endl;
        CloseGame();
    }
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

// EVENTS
int Engine::Init(){
    try {
        glfwSetErrorCallback(error_callback);

        // CONFIGURATION
        this->windowWidth = engine_config.resolution_window_size.x;
        this->windowHeight = engine_config.resolution_window_size.y;
        this->pixel_windowWidth = engine_config.resolution_pixels_size.x;
        this->pixel_windowHeight = engine_config.resolution_pixels_size.y;

        // INITIALIZATION
        glfwInit(); 
    
        #if defined(__APPLE__) // If using Apple device, use apple version
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set minimum major
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);// set minimum minor
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        srand((unsigned)time(NULL) * Utils::RandInt(0,240000));

        glfwWindowHint(GLFW_RESIZABLE,               engine_config.glfw_resizeable);
        glfwWindowHint(GLFW_DECORATED,               engine_config.glfw_decorated);
        glfwWindowHint(GLFW_FLOATING,                engine_config.glfw_topmost);
        glfwWindowHint(GLFW_MAXIMIZED,               engine_config.glfw_maximized);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, engine_config.glfw_transparent);
        glfwWindowHint(GLFW_DEPTH_BITS,              24);
    
        window = glfwCreateWindow(windowWidth, windowHeight, engine_config.window_name.c_str(), NULL, NULL);
        if (window == NULL)
        {
            throw std::runtime_error("Failed to create GLFW window");
            return -1;
        }
        glfwSetFramebufferSizeCallback(window, ResizeLoop);  
        glfwMakeContextCurrent(window);

        if (!engine_config.glfw_show_cursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // HIDE CURSOR
            auto center = GetWindowSize() / 2.0f;
            glfwSetCursorPos(GetWindow(), center.x, center.y);
        }

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // GLAD INIT, GL Context init
        {
            throw std::runtime_error("Failed to initialize GLAD");
            return -1;
        }    
        #ifdef DEBUG
            std::cout << "GL Version " <<  glGetString(GL_VERSION) << " [Bloom " << ENGINE_CONFIGURATION << "]\n"; // Requires GL Context
        #endif

        stbi_set_flip_vertically_on_load(true);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        if(engine_config.rendering_face_culling)
            glEnable(GL_CULL_FACE);            // Disable Double Sided Polygons
        else
            glDisable(GL_CULL_FACE);           // Enable Double Sided Polygons

        if(engine_config.rendering_anti_aliasing)
            glEnable(GL_MULTISAMPLE);          // ENABLE ANTI-ALIASING
        else
            glDisable(GL_MULTISAMPLE);          // Disable ANTI-ALIASING

        if(engine_config.glfw_use_depth)
            glEnable(GL_DEPTH_TEST);            // ENABLE DEPTH TESTING
        else
            glDisable(GL_DEPTH_TEST);           // DISABLE DEPTH TESTING

        glDepthFunc(GL_LESS);
        glClearDepth(1.0);
        glDepthMask(GL_TRUE);

        glfwSwapInterval(engine_config.rendering_enable_vsync);    // Enble VSYNC
    
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window,true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // CREATE PROJ MATRICES
        pixel_projection_matrix = glm::ortho(0.0f, (float)pixel_windowWidth, 0.0f, (float)pixel_windowHeight, (float)-1000, (float)1000); // ORTHO PROJECTION, window size you want to render at
        window_projection_matrix = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, (float)-1000, (float)1000); // ORTHO PROJECTION, window size you want to render at
        view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0)); 

        fb_pixels = new Bloom::Framebuffer(GetPixelWindowSize(), false);

        soloud.init();

        glfwSetWindowUserPointer(GetWindow(), this);

        GLFWimage images[1];
        images[0].pixels = stbi_load(engine_config.icon_path, &images[0].width, &images[0].height, 0, 4); //rgba channels 
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);

        steam_manager = new SteamTools::SteamManager();
        debug_sprite = new Bloom::Sprite("include/Bloom/res/engine/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix());

        this->font_alagard = new Bloom::Text("include/Bloom/res/engine/fonts/Alagard/alagard.ttf", GetViewMatrix(), GetWindowProjectionMatrix(),-0.5f,256);
        this->font_roboto= new Bloom::Text("include/Bloom/res/engine/fonts/Roboto/Roboto-Black.ttf", GetViewMatrix(), GetWindowProjectionMatrix(), -0.5f, 256);
        this->font_pressstart= new Bloom::Text("include/Bloom/res/engine/fonts/PressStart/PressStart.ttf", GetViewMatrix(), GetWindowProjectionMatrix(), -0.5f, 256);

        this->fb_bloom = new Bloom::Bloom_Framebuffer(GetWindowSize());

        glClearColor(0.1, 0.1, 0.1, 1.0); // THIS IS RESET BY THE SPLASH SCREEN
    }
    catch (const std::exception& ex) {
        std::cerr << "Encountered Bloom Exception during Initialization : " << ex.what() << std::endl;
        CloseGame();
        return -1;
    }
    return 0;
}

int Engine::Prep_Begin(){
    try {
        if(engine_config.feature_enable_parallel_loop)
            update_thread = new std::thread(&Engine::UpdateLoop, this);
        Begin();
    }
    catch (const std::exception& ex) {
            std::cerr << "Encountered Bloom Exception during Beginning : " << ex.what() << std::endl;
            CloseGame();
            return -1;
    }
    return 0;
}

int Engine::Prep_GameLoop(){
    try {
        if (!window_init_ready) {
            window_init_ready = true;
            glfwSetTime(0.0);
            return 0;
        }

        // PREPARE LOOP
        SteamTools::SteamManager::ProcessCallbacks();
        fps = 1000 / GetDeltaTime();

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
        cursor_position = glm::vec2(temp_cursor_position_x / (windowWidth / (float)pixel_windowWidth), pixel_windowHeight - temp_cursor_position_y / (windowHeight / (float)pixel_windowHeight) );

        // BINDING AND CLEARING PIXEL FRAMEBUFFER
        bindDefaultFramebuffer(true);

        if (!loaded){
            #ifndef DEBUG
                if (GetTime() < (splash_screen_time + 1.0)) {
                    this->font_pressstart->Print(GetWindowSize() / 2.0f, "BLOOM", 3, GetWindowSize(), glm::vec4(Utils::HSVtoRGB(glm::vec3(sin(GetTime() / splash_screen_time) * 360, 25, 100)), GetTime() < splash_screen_time ? 1 : 1 - (GetTime() - splash_screen_time)), true);
                    this->font_pressstart->Print(GetWindowSize() / 2.0f - glm::vec2(0, 100), "ENGINE", 0.5, GetWindowSize(), glm::vec4(glm::vec3(0.9), GetTime() < splash_screen_time ? 1 : 1 - (GetTime() - splash_screen_time)), true);
                } else {
                    loaded = true;
                }
            #else
                loaded = true;
            #endif // DEBUG
            glClearColor(0.0, 0.0, 0.0, 0.0);
        } else {
            GameLoop(); // ACTUAL GAME CODE
        }
    }
    catch (const std::exception& ex) {
            std::cerr << "Encountered Bloom Exception during GameLoop : " << ex.what() << std::endl;
            CloseGame();
            return -1;
    }
    return 0;
}

int Engine::Prep_UILoop(){
    try {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(ImGui_Cursor_Position.x, ImGui_Cursor_Position.y);
        ImGui::NewFrame();
        UILoop();
    }
    catch (const std::exception& ex) {
        std::cerr << "Encountered Bloom Exception during UILoop : " << ex.what() << std::endl;
        CloseGame();
        return -1;
    }
    return 0;
}

int Engine::EndLoop(){
    try {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Rendering pipeline end
        glfwSwapBuffers(window); // Render buffer to screen

        if (glfwWindowShouldClose(GetWindow()) && running) { // END IF SHOULD CLOSE
            return CloseGame();
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Encountered Bloom Exception during EndLoop : " << ex.what() << std::endl;
        CloseGame();
        return -1;
    }
    return 0;
}

void Game::Engine::UpdateLoop()
{
    while (!glfwWindowShouldClose(GetWindow())) {
        Update();
    }
    update_thread->detach();
    delete update_thread;
}

void Engine::Begin() {
    std::string errmsg = "Please define application method 'Begin'";
    throw std::runtime_error(errmsg);
}

void Engine::UILoop() {
    std::string errmsg = "Please define application method 'UILoop'";
    throw std::runtime_error(errmsg);
}

void Engine::GameLoop() {
    std::string errmsg = "Please define application method 'GameLoop'";
    throw std::runtime_error(errmsg);
}

int Game::Engine::CloseGame()
{
    running = false;

    soloud.deinit();
    SteamAPI_Shutdown();
    glfwDestroyWindow(GetWindow());
    glfwTerminate();

    delete fb_pixels;
    delete steam_manager;
    delete debug_sprite;

    std::cout << Utils::Default() << Utils::Default(true);
    return 0;
}

void Engine::InputLoop() {
    /*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);*/
}

glm::vec2 Engine::GetCursorWorldPosition(glm::vec2 camera_location)
{
    return (GetCursorPosition() - (GetPixelWindowSize() / glm::vec2(2)) + GetCameraWorldPosition(camera_location));
}

glm::vec2 Engine::GetCameraWorldPosition(glm::vec2 camera_location)
{
    return (GetPixelWindowSize() / glm::vec2(2)) - camera_location;
}

void Game::Engine::ExitGame()
{
    glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
}
