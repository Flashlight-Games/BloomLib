#include "App_Shooter.h"

using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application::Shooter* app = static_cast<Application::Shooter*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
         //   app->GetEmmiter()->Play(app->GetWindow(),app->GetTime(),app->GetCursorPosition());
        }
}

void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    Application::Shooter* app = static_cast<Application::Shooter*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        app->GetChecker()->SetPosition(app->GetCursorPosition(),app->GetWindow(),app->GetTime(),app->GetmainBox());
    }
}

void Shooter::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);

    text_pressstart = new Bloom::Text("res/engine/fonts/PressStart/PressStart.ttf",GetViewMatrix(),GetWindowProjectionMatrix(),-0.5, 512);

    sprite_block = new Bloom::Sprite("res/engine/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5));
    sprite_block->SetLocation(GetPixelWindowSize() / glm::vec2(2));
    sprite_block->SetRotation(0);

    sprite_block->GetShader()->use();
    block_color_uid = sprite_block->GetShader()->getUniformId("uColor");

    mainBox = new PhysX::collisionBox("main", sprite_block);

    checker = new Application::Checker(25,GetViewMatrix(),GetPixelProjectionMatrix());
}

void Shooter::UILoop() {
    if (Utils::isDevelopmentConfiguration()) {
        ImGui::Begin("Control window");
        ImGui::ColorPicker3("BLOCK COLOR",block_color);
        ImGui::End();

        ImGui::Begin("Performance window");
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
        ImGui::End();
    }
        
    text_pressstart->Print(glm::vec2(10, GetWindowSize().y - 25), ("FPS:" + std::to_string(GetFPS())).c_str(), 0.25, GetWindowSize(), glm::vec4(1, 1, 1, 1));
}

void Shooter::GameLoop() {
    EnablePixels();

    sprite_block->GetShader()->use();
    sprite_block->GetShader()->uniformVec4(block_color_uid, glm::vec4(block_color[0], block_color[1], block_color[2], 1));
    sprite_block->SetRotation((GetTime() / 10.0) * 360);
    sprite_block->Draw(GetViewMatrix());

    mainBox->setLocation(sprite_block->GetLocation());
    mainBox->setRotation(sprite_block->GetRotation());
    mainBox->setScale(sprite_block->GetSize());

    checker->Draw(GetTime(), GetGameSpeed());
    DrawPixelsToDefault();
}

void Shooter::InputLoop() {
    
}