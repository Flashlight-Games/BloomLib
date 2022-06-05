#include "App_Boxer.h"
#include "../../src/engine/engine.h"

using namespace Application;

void Boxer::Begin() {
    srand(time(NULL)); // SEED RANDOM

    player = new Application::Player(this);

    const char* fontPath = "res/fonts/PressStart/PressStart.ttf";
    timer = new Bloom::Text(fontPath,GetViewMatrix(),GetWindowProjectionMatrix(),-0.5, 512);

    protect = new Bloom::Sprite("res/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5, 0.5), glm::vec2(0.625*2));
    protect->SetLocation(glm::vec2(GetPixelWindowSize().x / 2, GetPixelWindowSize().y / 2));

    move_circle_buffer = new Bloom::Framebuffer(GetPixelWindowSize().x, GetPixelWindowSize().y, false);
    move_circle_buffer->setShader("res/app_boxer/shaders/framebuffer_move_circle.glsl");
    
    circle_data_uid = move_circle_buffer->GetShader().getUniformId("uData");
    circle_time_uid = move_circle_buffer->GetShader().getUniformId("uTime");
    circle_position_uid = move_circle_buffer->GetShader().getUniformId("uPosition");
    protect_color_uid = protect->GetShader().getUniformId("uColor");

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

    timer->Print(glm::vec2(50, 50), std::to_string((int)GetTime()), 1, GetWindowSize(),((int)GetTime() % 10 == 0 && GetTime() >= 10) ? glm::vec4(0.15, 0.15, 0.15, 1) : glm::vec4(0.15, 0.15, 0.15, 1));
    timer->Print(glm::vec2(10, GetWindowSize().y-25), ("FPS:" + std::to_string(GetFPS())).c_str(), 0.25, GetWindowSize(), glm::vec4(0.15, 0.15, 0.15, 1));
    timer->Print(glm::vec2(10, GetWindowSize().y - 50), ("VEL:" + std::to_string(player->GetVelocity())).c_str(), 0.25, GetWindowSize(), glm::vec4(0.15, 0.15, 0.15, 1));
}

void Boxer::GameLoop() {
    EnablePixels();

    DrawBackground();

    DrawProtect();
    DrawPlayer();

    if (ShouldSpawnEnemies()) {
        SpawnEnemies(EnemySpawnAmount());
    }

    ProcessEnemies();
    DrawEnemies();

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
    if (!time)
        spawned_wave = false;
    return time && !spawned_wave;
}

int Boxer::EnemySpawnAmount() {
    return (int)GetTime() / 10 * (int)(GetTime() >= 10);
}

void Boxer::SpawnEnemies(int amount) {
    int variation = 33;

    for (int i = 0; i < amount; i++)
    {
        int spawn_radius = 72*(1+(float)i) + rand() % variation;
        int angle = rand() % 360;
        glm::vec2 position = glm::vec2(
            (spawn_radius * cos(angle) + GetPixelWindowSize().x / 2),
            (spawn_radius * sin(angle) + GetPixelWindowSize().x / 2));

        enemies.push_back(new Application::Enemy(this, position));
    }
    spawned_wave = true;
}

void Boxer::ProcessEnemies() {
    for (size_t i = 0; i < enemies.size(); i++) {
        enemies.at(i)->SetPosition(this);
        if (Utils::Distance2D(enemies.at(i)->GetPosition(), glm::vec2(GetPixelWindowSize().x / 2, GetPixelWindowSize().y / 2)) < 10) {
            Damage();
            enemies.erase(enemies.begin() + i);
        }
    }
}

void Boxer::DrawEnemies() {
    for (size_t i = 0; i < enemies.size(); i++) {
        enemies.at(i)->Draw(this);
    }
}

void Boxer::Damage() {
    health -= 1;
    if (health <= 0)
        std::exit(0);
    camera_location += glm::vec2(100, 100);
    SetCameraLocation(camera_location);
}

void Boxer::DrawProtect() {
    float rotation = GetTime() * 36;
    protect->SetRotation(rotation);

    protect->GetShader().use();
    protect->GetShader().uniformVec4(protect_color_uid,Utils::Lerp(glm::vec4(0.9, 0.97, 1, 1),glm::vec4(0.1, 0.775, 1,1),(health/3.0)));
    protect->Draw(GetViewMatrix());
}