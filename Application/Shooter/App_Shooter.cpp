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

    framebuffer_ca = new Bloom::Chromatic_Aberration(GetWindowSize().x, GetWindowSize().y);
    framebuffer_bloom = new Bloom::Bloom_Framebuffer(GetWindowSize().x, GetWindowSize().y);

    text_pressstart = new Bloom::Text("res/engine/fonts/PressStart/PressStart.ttf",GetViewMatrix(),GetWindowProjectionMatrix(),-0.5, 512);

    sprite_block = new Bloom::Sprite("res/test/ring.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5));
    sprite_block->SetLocation(GetPixelWindowSize() / glm::vec2(2));
    sprite_block->SetRotation(0);

    sprite_block->GetShader()->use();
    block_color_uid = sprite_block->GetShader()->getUniformId("uColor");
}

void Shooter::UILoop() {
    if (Utils::isDevelopmentConfiguration()) {
        ImGui::Begin("Control window");
        ImGui::ColorPicker3("BLOCK COLOR",block_color);
        ImGui::DragFloat("CA SIZE", &ca_size,0.001,0.0,0.1);
        ImGui::DragInt("BLOOM", &bloom_size, 1, 0.0, 100.0);
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
    sprite_block->Draw(GetViewMatrix());

    
    framebuffer_bloom->Use(true);
    framebuffer_bloom->SetSize(bloom_size);
    DrawPixels();

    //sprite_block->SetRotation((GetTime()/10.0)*360);

    framebuffer_bloom->DrawToDefault(GetWindowSize());
}

void Shooter::InputLoop() {
    
}