#include "App.h"

//using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = Utils::GetApp<Application::App>(window);
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            app->ExitGame();
        }
        break;
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
    this->test->Draw();
    DrawPixelsToDefault();
}

void Application::App::InputLoop() {
    //pass
    if (glfwGetKey(GetWindow(), GLFW_KEY_LEFT))
        cam_loc += glm::vec2(5*(1/1000.0), 0);
    if (glfwGetKey(GetWindow(), GLFW_KEY_RIGHT))
        cam_loc += glm::vec2(-5*(1 / 1000.0), 0);
    if (glfwGetKey(GetWindow(), GLFW_KEY_UP))
        cam_loc += glm::vec2(0,-5 * (1 / 1000.0));
    if (glfwGetKey(GetWindow(), GLFW_KEY_DOWN))
        cam_loc += glm::vec2(0,5 * (1 / 1000.0));
    if (glfwGetKey(GetWindow(), GLFW_KEY_Q))
        this->test->SetRotation(this->test->GetRotation() + 0.01);
    this->SetCameraLocation(cam_loc);
}

void Application::App::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);
    this->test = new Bloom::Sprite("include/Bloom/res/engine/textures/detailTest16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix());
    this->test->SetLocation(glm::vec2(20));

    update_thread = new std::thread(&Application::App::Update, this);
    glClearColor(0.21, 0.21, 0.21, 1.0);
}

void Application::App::Update() {
    while (!glfwWindowShouldClose(GetWindow())) {

    }

    std::terminate();
    delete update_thread;
}

Application::App::~App()
{

}
