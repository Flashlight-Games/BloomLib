#include "engine.h"

using namespace Game;

// GLFW RESIZE
void Game::ResizeLoop(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

// BACKEND
void Engine::InputLoop(GLFWwindow* window){
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        {glfwSetWindowShouldClose(window,true);} 
    // locked = glfwGetKey(window,GLFW_KEY_L) == GLFW_PRESS;
} // Close window

// EVENTS
int Engine::Init(){
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
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
    
    window = glfwCreateWindow(windowWidth, windowHeight, "Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, ResizeLoop);  
    glfwMakeContextCurrent(window);
    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // GLAD INIT, GL Context init
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
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
    view_matrix = glm::mat4(1.0f);

    auto tmp_pixels = Bloom::Framebuffer(pixel_windowWidth,pixel_windowHeight,false);
    tmp_pixels.setTextureUniform("uTexture");

    fb_pixels = &tmp_pixels;
    return Begin();
}

int Engine::Begin(){
    view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, -1.0f, 0.0f)); 

    auto tmp_sprite = Bloom::Sprite("res/textures/detailTest16Line.png",1,view_matrix,pixel_projection_matrix);
    spritetest = &tmp_sprite;
    auto tmp_sheet = Bloom::Spritesheet("res/textures/2x8_SpritesheetTest.png",1,glm::vec2(2,8),view_matrix,pixel_projection_matrix);
    sheettest = &tmp_sheet;

    glClearColor(0.21,0.21,0.21,1);

    int retval = 0;
    while (retval == 0){
        retval = GameLoop();
    }

    return retval;
}

int Engine::GameLoop(){
    if(glfwWindowShouldClose(window)) // END IF SHOULD CLOSE
        return -2;

    // PREPARE LOOP
    fps = ImGui::GetIO().Framerate;
    glfwPollEvents(); InputLoop(window);
    if(!isLocked)
            time = glfwGetTime();


    // BINDING AND CLEARING PIXEL FRAMEBUFFER
    (*fb_pixels).use(true);

    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-
    (*spritetest).SetLocation(glm::vec2(boxPosX,boxPosY));
    (*spritetest).SetRotation(sin(time)*180);
    (*spritetest).Draw();

    (*sheettest).Draw();
    (*sheettest).TimedAdvance(0.1);
    
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-
    (*fb_pixels).drawToDefault(windowWidth,windowHeight);

    return UILoop();
}

int Engine::UILoop(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Control window");
        ImGui::TextColored(ImVec4(1,0.5,0,1),"Time is %s",std::to_string(((int)time)).c_str());
        ImGui::SliderInt("Box PosX",&boxPosX,0,pixel_windowWidth);
        ImGui::SliderInt("Box PosY",&boxPosY,0,pixel_windowHeight);
    ImGui::End();

    ImGui::Begin("Performance window");
    // //     ImGui::ProgressBar(transform_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Transform");
    // //     ImGui::ProgressBar(frameBuffer_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Framebuffer");
    // //     ImGui::ProgressBar(imGui_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"ImGui");
    // //     ImGui::ProgressBar(font_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Font");

    // //     ImGui::ProgressBar(bufferSwap_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"BufferSwap");
        ImGui::TextColored(ImVec4(1,1,0,1),"%s FPS",std::to_string(fps).c_str());
    ImGui::End();
    return EndLoop();
}

int Engine::EndLoop(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Rendering pipeline end
    glfwSwapBuffers(window); // Render buffer to screen
    return 0;
}