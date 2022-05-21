#include "App_Boxer.h"
#include "../../src/engine/engine.h"

using namespace Application;

void Boxer::Begin() {
    Bloom::Sprite* tmp_e_box = new Bloom::Sprite("res/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix());
    endBox = tmp_e_box;

    const char* fontPath = "C:/Windows/Fonts/arial.ttf";
    timer = new Bloom::Text(fontPath, 16);
    
    std::cout << "size: " << sizeof(*timer) << "\n";
    glClearColor(0.0, 0.0, 0.0, 1.0f); // Set Background Color
}

void Boxer::UILoop() {
    /*ImGui::Begin("Control window");
    ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
    ImGui::SliderFloat2("A Box Pos", ABoxPos, 0, GetPixelWindowSize().x);
    ImGui::SliderFloat2("B Box Pos", BBoxPos, 0, GetPixelWindowSize().x);
    ImGui::End();*/

    ImGui::Begin("Performance window");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
    ImGui::Image((void*)timer->getTexture()->getID(), ImVec2(512, 512));
    ImGui::End();

    timer->Print(glm::vec2(0.1, 960/2), "A B C", GetWindowSize());
}

void Boxer::GameLoop() {
    


    BBoxPos[0] += (BBoxPos[0]-GetCursorPosition().x) * -0.05f; BBoxPos[1] += (BBoxPos[1]-GetCursorPosition().y) * -0.05f;

    endBox->SetLocation(glm::vec2((int)BBoxPos[0], (int)BBoxPos[1]));
    endBox->SetRotation(atan2(BBoxPos[1] - GetCursorPosition().y, BBoxPos[0] - GetCursorPosition().x) * 180.0f / 3.1416f);
    endBox->SetScale(glm::vec2(0.1,0.1));

    endBox->GetShader().use();
    endBox->GetShader().uniformVec4(endBox->GetShader().getUniformId("uColor"), glm::vec4(1, 0.5, 0, 1));
    endBox->Draw(GetViewMatrix());
}

void Boxer::InputLoop() {
    if (glfwGetKey(GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(GetWindow(), true);
}