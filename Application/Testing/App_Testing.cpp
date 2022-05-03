#include "App_Testing.h"

using namespace Application;

void Testing::Begin() {
    Bloom::Sprite* tmp_e_box = new Bloom::Sprite("res/textures/detailTest16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5, 0), glm::vec2(0.25, 0.25));
    endBox = tmp_e_box;
}

void Testing::UILoop() {
    ImGui::Begin("Control window");
    ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
    ImGui::SliderInt2("A Box Pos", ABoxPos, 0, GetPixelWindowSize().x);
    ImGui::SliderInt2("B Box Pos", BBoxPos, 0, GetPixelWindowSize().x);
    ImGui::End();

    ImGui::Begin("Performance window");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
    ImGui::End();
}

void Testing::GameLoop() {
    endBox->SetLocation(glm::vec2(BBoxPos[0], BBoxPos[1]));
    endBox->GetShader().use();
    endBox->GetShader().uniformVec4(endBox->GetShader().getUniformId("uColor"), glm::vec4(1, 0.5, 0, 1));
    endBox->Draw(GetViewMatrix());
}