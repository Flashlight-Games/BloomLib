#include "App.h"

//using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = Utils::GetApp<Application::App>(window);
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            app->CloseGame();
            delete app;
        }
        break;
    case GLFW_KEY_UP:
        if (action == GLFW_PRESS)
            Utils::EnableWireframe();
        else if(action == GLFW_RELEASE)
            Utils::DisableWireframe();
    }
}

void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    auto app = Utils::GetApp<Application::App>(window);
    switch (key) {
    case GLFW_MOUSE_BUTTON_LEFT:
        break;
    }
}

void Application::App::UILoop() {
    ImGui::Begin("Control window"); {
        if (Utils::isDevelopmentConfiguration()) {
            ImGui::TextColored(ImVec4(1, 0.5, 0.0, 1), (std::string("GLFW v_").c_str() + std::string(glfwGetVersionString())).c_str());
        }
    } ImGui::End();

    if (Utils::isDevelopmentConfiguration()) {
        ImGui::Begin("Performance window");
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
        ImGui::End();
    }
}

void Application::App::GameLoop() {
    //EnablePixels();
    std::vector<glm::vec2> offsets;
    for (int idxY = 0; idxY < this->testSprite->GetVerticesAmount().y; idxY++)
    {
        for (int idxX = 0; idxX < this->testSprite->GetVerticesAmount().x; idxX++)
        {
            offsets.push_back(glm::vec2(glm::sin(2*GetTime()) * glm::sin(0.5*GetTime()) * glm::pow(1 - (idxY+1) / this->testSprite->GetVerticesAmount().y,2) * 100, 0));
        }
    }

    this->testSprite->SetVertexPositionOffsets(offsets);

    testSprite->Draw();
    //DrawPixelsToDefault();
}

void Application::App::InputLoop() {
    //pass
}

void Application::App::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);

    this->testSprite = new Bloom::SubdividedSprite("include/Bloom/res/engine/textures/grass.png",1,GetViewMatrix(),GetWindowProjectionMatrix(),glm::vec2(4));
    this->testSprite->SetLocation(GetWindowSize() / glm::vec2(2.0));
    this->testSprite->SetScale(glm::vec2(1));
    this->testSprite->SetShader(new Bloom::Shader("include/Bloom/res/engine/shaders/sprite/spriteshader_vertex.glsl", "include/Bloom/res/engine/shaders/sprite/spriteshader_fragment_UV.glsl"), GetWindowProjectionMatrix());
    
    

    update_thread = new std::thread(&Application::App::Update, this);
    glClearColor(0.21, 0.21, 0.21, 1.0);
}

void Application::App::Update() {
    while (!glfwWindowShouldClose(GetWindow())) {

    }

    update_thread->detach();
    delete update_thread;
}

Application::App::~App()
{
    delete testSprite;
    delete referenceSprite;
}
