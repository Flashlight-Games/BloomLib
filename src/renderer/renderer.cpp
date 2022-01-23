#include "renderer.h"

#include <filesystem>


using namespace Bloom;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{ 
    glViewport(0, 0, width, height); // Resize viewport with window
};

Renderer::Renderer(glm::vec2 window_size,glm::vec2 pixel_size,void (*mainLoop)(), void (*inputLoop)(), void (*uiLoop)(),std::string windowName,bool resizeable,
bool borderless,bool maximised, bool transparent){

    this->MainLoop = mainLoop; this->InputLoop = inputLoop; this->UILoop = uiLoop;

    // INITIALIZATION
    glfwInit(); 
    
    #if defined(__APPLE__) // If using Apple device, use apple version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set minimum major
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);// set minimum minor
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    uint16_t windowWidth = window_size[0]; uint16_t windowHeight = window_size[1];
    uint16_t pixel_windowWidth = pixel_size[0]; uint16_t pixel_windowHeight = pixel_size[1];

    this->windowSize = window_size; this->pixelSize = pixel_size;

    glfwWindowHint(GLFW_RESIZABLE, resizeable); // Set window not resizable
    glfwWindowHint(GLFW_DECORATED, !borderless);
    glfwWindowHint(GLFW_MAXIMIZED, maximised);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, transparent);

    this->window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // GLAD INIT, GL Context init
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }    
    std::cout << "GL Version " <<  glGetString(GL_VERSION) << std::endl; // Requires GL Context
    stbi_set_flip_vertically_on_load(true);

    float nearClip = -1000;
    float farClip = 1000;
    this->pixelProjection = glm::ortho(0.0f, pixelSize[0], 0.0f, pixelSize[1], nearClip, farClip); // ORTHO PROJECTION, window size you want to render at
    this->projection = glm::ortho(0.0f, windowSize[0], 0.0f, windowSize[1], nearClip, farClip); // ORTHO PROJECTION, window size you want to render at

    glm::mat4 view_matrix = glm::mat4(1.0f);
    view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, -1.0f, 0.0f)); 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE); // Enable Double Sided Polygons
    glDisable(GL_MULTISAMPLE); // ENABLE ANTI-ALIASING
    // glfwSwapInterval(0); // DISABLE VSYNC
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // PROCESSING //
    Bloom::Framebuffer pixelbuffer = Bloom::Framebuffer(pixelSize[0],pixelSize[1],false);
    this->fb_pixels = &pixelbuffer;

    this->fb_pixels->setShader("src/shaders/framebuffer/framebuffer_fragment.glsl");
    this->fb_pixels->fbShader.uniformInt(fb_pixels->fbShader.getUniformId("uTexture"),fb_pixels->getTexture().getID());

}

void Renderer::Update(){
    this->fps = ImGui::GetIO().Framerate;

    glfwSwapBuffers(window); // Render buffer to screen
    glfwPollEvents(); 
    if(InputLoop != nullptr)
        this->InputLoop();

    if(!locked)
            time = glfwGetTime();

    // BINDING PIXEL FRAMEBUFFER
    fb_pixels->use(true);
    fb_pixels->fbShader.use();

    if(MainLoop != nullptr)
        this->MainLoop();

    bindDefaultFramebuffer(true,pixelSize[0],pixelSize[1]);
    fb_pixels->draw();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if(UILoop != nullptr)
        UILoop();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

float Renderer::GetTime(){
    return this->time;
}

int Renderer::GetFPS(){
    return this->fps;
}

bool Renderer::GetLocked(){
    return this->locked;
}

void Renderer::SetLocked(bool locked){
    this->locked = locked;
}

glm::vec2 Renderer::GetWindowSize(){
    return this->windowSize;
}

glm::vec2 Renderer::GetPixelSize(){
    return this->pixelSize;
}

GLFWwindow* Renderer::GetWindow(){
    return this->window;
}

glm::mat4 Renderer::getProjectionMatrix(){
    return this->projection;
}

glm::mat4 Renderer::getPixelProjectionMatrix(){
    return this->pixelProjection;
}

glm::mat4 Renderer::getViewMatrix(){
    return this->view;
}
