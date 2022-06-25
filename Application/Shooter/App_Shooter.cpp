#include "App_Shooter.h"

using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //Application::Boxer* app = static_cast<Application::Boxer*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            // PASS
        }
}


void Shooter::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);

    const char* fontPath = "res/engine/fonts/PressStart/PressStart.ttf";
    text_pressstart = new Bloom::Text(fontPath,GetViewMatrix(),GetWindowProjectionMatrix(),-0.5, 512);
}

void Shooter::UILoop() {
    if (Utils::isDevelopmentConfiguration()) {
        /*ImGui::Begin("Control window");
        ImGui::End();*/

        ImGui::Begin("Performance window");
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
        ImGui::End();
    }
        
    text_pressstart->Print(glm::vec2(10, GetWindowSize().y - 25), ("FPS:" + std::to_string(GetFPS())).c_str(), 0.25, GetWindowSize(), glm::vec4(0.15, 0.15, 0.15, 1));
}

void Shooter::GameLoop() {
    EnablePixels();

    DrawPixelsToDefault();
}

void Shooter::InputLoop() {
    
}