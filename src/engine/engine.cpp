#include "engine.h"

using namespace Game;


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
        std::cout << "Encountered Bloom Exception during InputLoop : " << ex.what() << std::endl;
        glfwTerminate();
    }
}

// EVENTS
int Engine::Init(){
    try {
        // INITIALIZATION
        glfwInit(); 
    
        #if defined(__APPLE__) // If using Apple device, use apple version
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set minimum major
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);// set minimum minor
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        glfwWindowHint(GLFW_RESIZABLE,               GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED,               GLFW_TRUE); 
        glfwWindowHint(GLFW_FLOATING,                GLFW_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED,               GLFW_FALSE);
        glfwWindowHint(GLFW_CENTER_CURSOR,           GLFW_FALSE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    
        window = glfwCreateWindow(windowWidth, windowHeight, "Window", NULL, NULL);
        if (window == NULL)
        {
            throw std::runtime_error("Failed to create GLFW window");
            return -1;
        }
        glfwSetFramebufferSizeCallback(window, ResizeLoop);  
        glfwMakeContextCurrent(window);
    

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // GLAD INIT, GL Context init
        {
            throw std::runtime_error("Failed to initialize GLAD");
            return -1;
        }    
        std::cout << "GL Version " <<  glGetString(GL_VERSION) << std::endl; // Requires GL Context
        stbi_set_flip_vertically_on_load(true);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_CULL_FACE);    // Enable Double Sided Polygons
        glDisable(GL_MULTISAMPLE);  // ENABLE ANTI-ALIASING
        glfwSwapInterval(true);     // Enble VSYNC
    
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window,true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // CREATE PROJ MATRICES
        pixel_projection_matrix = glm::ortho(0.0f, (float)pixel_windowWidth, 0.0f, (float)pixel_windowHeight, (float)-1000, (float)1000); // ORTHO PROJECTION, window size you want to render at
        window_projection_matrix = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, (float)-1000, (float)1000); // ORTHO PROJECTION, window size you want to render at
        view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0)); 

        Bloom::Framebuffer* tmp_pixels = new Bloom::Framebuffer(pixel_windowWidth, pixel_windowHeight, false);
        tmp_pixels->setTextureUniform("uTexture");

        fb_pixels = tmp_pixels;
    }
    catch (const std::exception& ex) {
        std::cout << "Encountered Bloom Exception during Initialization : " << ex.what() << std::endl;
        glfwTerminate();
        return -1;
    }
    return 0;
}

int Engine::Prep_Begin(){
    try {
        Begin();
    }
    catch (const std::exception& ex) {
            std::cout << "Encountered Bloom Exception during Beginning : " << ex.what() << std::endl;
            glfwTerminate();
            return -1;
    }
    return 0;
}

int Engine::Prep_GameLoop(){
    try {
        view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // SET CAMERA POSITION

        if (glfwWindowShouldClose(window)) // END IF SHOULD CLOSE
            return -2;

        // PREPARE LOOP
        fps = ImGui::GetIO().Framerate;
        glfwPollEvents(); Prep_InputLoop();
        if (!isLocked)
            time = glfwGetTime();

        double temp_cursor_position_x;
        double temp_cursor_position_y;
        glfwGetCursorPos(window, &temp_cursor_position_x, &temp_cursor_position_y);
        cursor_position = glm::vec2(temp_cursor_position_x / (windowWidth / (float)pixel_windowWidth), pixel_windowHeight - temp_cursor_position_y / (windowHeight / (float)pixel_windowHeight) );

        // BINDING AND CLEARING PIXEL FRAMEBUFFER
        (*fb_pixels).use(true);

        GameLoop(); // ACTUAL GAME CODE
        
        (*fb_pixels).drawToDefault(windowWidth, windowHeight);
    }
    catch (const std::exception& ex) {
            std::cout << "Encountered Bloom Exception during GameLoop : " << ex.what() << std::endl;
            glfwTerminate();
            return -1;
    }
    return 0;
}

int Engine::Prep_UILoop(){
    try {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UILoop();
    }
    catch (const std::exception& ex) {
        std::cout << "Encountered Bloom Exception during UILoop : " << ex.what() << std::endl;
        glfwTerminate();
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
    }
    catch (const std::exception& ex) {
        std::cout << "Encountered Bloom Exception during EndLoop : " << ex.what() << std::endl;
        glfwTerminate();
        return -1;
    }
    return 0;
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

void Engine::InputLoop() {
    /*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);*/
}