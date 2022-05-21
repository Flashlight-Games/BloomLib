#include "App_Testing.h"
#include "../../src/engine/engine.h"

using namespace Application;

void Testing::Begin() {
    Bloom::Sprite* tmp_e_box = new Bloom::Sprite("res/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5, 0.5), glm::vec2(0.25, 0.25));
    endBox = tmp_e_box;

    glClearColor(0.21, 0.21, 0.21, 1.0f); // Set Background Color
}

void Testing::UILoop() {
    ImGui::Begin("Control window");
    ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
    ImGui::SliderFloat2("A Box Pos", ABoxPos, 0, GetPixelWindowSize().x);
    ImGui::SliderFloat2("B Box Pos", BBoxPos, 0, GetPixelWindowSize().x);
    ImGui::End();

    ImGui::Begin("Performance window");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
    ImGui::End();
}

void Testing::GameLoop() {
    endBox->SetLocation(glm::vec2((int)BBoxPos[0], (int)BBoxPos[1]));
    endBox->SetRotation(atan2(BBoxPos[1] - GetCursorPosition().y, BBoxPos[0] - GetCursorPosition().x) * 180 / 3.1416);

    endBox->GetShader().use();
    endBox->GetShader().uniformVec4(endBox->GetShader().getUniformId("uColor"), glm::vec4(1, 0.5, 0, 1));
    endBox->Draw(GetViewMatrix());
}

void Testing::InputLoop() {
    if (glfwGetKey(GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(GetWindow(), true);
}