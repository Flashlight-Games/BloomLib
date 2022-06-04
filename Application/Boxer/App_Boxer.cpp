#include "App_Boxer.h"
#include "../../src/engine/engine.h"

using namespace Application;

void Boxer::Begin() {
    player = new Application::Player(this);

    //const char* fontPath = "C:/Windows/Fonts/arial.ttf";
    //const char* fontPath = "res/fonts/Alagard/alagard.ttf";
    const char* fontPath = "res/fonts/PressStart/PressStart.ttf";
    timer = new Bloom::Text(fontPath,GetViewMatrix(),GetWindowProjectionMatrix(),-0.5, 512);

    move_circle_buffer = new Bloom::Framebuffer(GetPixelWindowSize().x, GetPixelWindowSize().y, false);
    move_circle_buffer->setShader("Application/Boxer/shaders/framebuffer_move_circle.glsl");
    
    circle_data_uid = move_circle_buffer->GetShader().getUniformId("data");
    circle_time_uid = move_circle_buffer->GetShader().getUniformId("time");
    circle_position_uid = move_circle_buffer->GetShader().getUniformId("position");

    glClearColor(0.21, 0.21, 0.21, 1.0f); // Set Background Color
}

void Boxer::UILoop() {
    /*ImGui::Begin("Control window");
    ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
    ImGui::SliderFloat2("A Box Pos", ABoxPos, 0, GetPixelWindowSize().x);
    ImGui::SliderFloat2("B Box Pos", BBoxPos, 0, GetPixelWindowSize().x);
    ImGui::End();*/

    ImGui::Begin("Performance window");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
    ImGui::End();

    timer->Print(glm::vec2(50, 50), std::to_string((int)GetTime()), 1, GetWindowSize(),glm::vec4(0.15,0.15,0.15,1));
    timer->Print(glm::vec2(10, GetWindowSize().y-25), ("FPS:" + std::to_string(GetFPS())).c_str(), 0.25, GetWindowSize(), glm::vec4(0.15, 0.15, 0.15, 1));
    timer->Print(glm::vec2(10, GetWindowSize().y - 50), ("VEL:" + std::to_string(player->GetVelocity())).c_str(), 0.25, GetWindowSize(), glm::vec4(0.15, 0.15, 0.15, 1));
}

void Boxer::GameLoop() {
    EnablePixels();

    DrawBackground();
    DrawPlayer();

    if (ShouldSpawnEnemies()) {
        SpawnEnemies(EnemySpawnAmount());
    }

    DrawPixels();
}

void Boxer::InputLoop() {
    if (glfwGetKey(GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(GetWindow(), true);
}



void Boxer::DrawBackground() {
    move_circle_buffer->GetShader().use();
    move_circle_buffer->GetShader().uniformVec3(circle_data_uid, glm::vec3(GetPixelWindowSize().x, GetPixelWindowSize().y, move_radius));
    move_circle_buffer->GetShader().uniformVec2(circle_position_uid, player->GetPosition());
    move_circle_buffer->GetShader().uniformFloat(circle_time_uid, GetTime());
    move_circle_buffer->draw();
}

void Boxer::DrawPlayer() {
    player->SetPosition(this, move_radius);
    player->Draw(this);
}


bool Boxer::ShouldSpawnEnemies() {
    bool time = (int)GetTime() % 10 == 0 && GetTime() >= 10;
    if (!time && spawned_wave)
        spawned_wave = false;
    return time && !spawned_wave;
}

int Boxer::EnemySpawnAmount() {
    return (int)GetTime() / 10 * (int)(GetTime() >= 10);
}

void Boxer::SpawnEnemies(int amount) {
    std::cout << "SPAWNED " << amount << "\n";
    spawned_wave = true;
}