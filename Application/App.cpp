#include "App.h"

//using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = Utils::GetApp<Application::App>(window);
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            app->CloseGame();
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
    glClearColor(0.21, 0.21, 0.21, 1.0);
    DrawPixelsToDefault(false);
}

void Application::App::InputLoop() {
    //pass
}

void Application::App::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);

    update_thread = new std::thread(&Application::App::Update, this);
}


void Application::App::Update() {
    while (!glfwWindowShouldClose(GetWindow())) {

    }

    update_thread->detach();
    delete update_thread;
}
