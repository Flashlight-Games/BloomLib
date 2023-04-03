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
    // RENDERING
    EnablePixels();
    DrawPixelsToDefault();
}

void Application::App::InputLoop() {
    //pass
}

void Application::App::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);
    
    this->test = new Bloom::Sprite("include/Bloom/res/engine/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix());
    this->cstest = new Bloom::ComputeShader("include/Bloom/res/engine/shaders/compute/compute_shader.glsl", glm::vec2(10, 1));

    this->cstest->Use();
    this->cstest->SetValues(std::vector<float>({0,1,2,3,4,5,6,7,8,9}));

    for (int i = 0; i < 10; i++)
    {
        this->cstest->Use();
        this->cstest->Dispatch(true);
        auto data = this->cstest->ReadData();

        for (int i = 0; i < data.size(); i++)
        {
            std::cout << data.at(i) << ", ";
        }
        std::cout << "\n";
    }

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

}
