#include "App_Boxer.h"

using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application::Boxer* app = static_cast<Application::Boxer*>(glfwGetWindowUserPointer(window));
    if (app->GetInMenu()) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            app->CloseGame(window);
        }
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            app->ResetGame();
        }
    }
    else {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            // PASS
        }
    }
}


void Boxer::Begin() {
    srand(time(NULL)); // SEED RANDOM
    glfwSetKeyCallback(GetWindow(), key_callback);

    player = new Application::Player(this);

    const char* fontPath = "res/engine/fonts/PressStart/PressStart.ttf";
    timer = new Bloom::Text(fontPath,GetViewMatrix(),GetWindowProjectionMatrix(),-0.5, 512);

    protect = new Bloom::Sprite("res/engine/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5, 0.5), glm::vec2(0.625*2));
    protect->SetLocation(glm::vec2(GetPixelWindowSize().x / 2, GetPixelWindowSize().y / 2));

    move_circle_buffer = new Bloom::Framebuffer(GetPixelWindowSize().x, GetPixelWindowSize().y, false);
    move_circle_buffer->setShader("res/app_boxer/shaders/framebuffer_move_circle.glsl");
    circle_data_uid = move_circle_buffer->GetShader().getUniformId("uData");
    circle_time_uid = move_circle_buffer->GetShader().getUniformId("uTime");
    circle_position_uid = move_circle_buffer->GetShader().getUniformId("uPosition");
    protect_color_uid = protect->GetShader()->getUniformId("uColor");


    wave_buffer = new Bloom::Framebuffer(GetPixelWindowSize().x, GetPixelWindowSize().y, false);
    wave_buffer->setShader("res/app_boxer/shaders/framebuffer_wave.glsl");
    wave_radius_uid = wave_buffer->GetShader().getUniformId("uRadius");
    wave_center_uid = wave_buffer->GetShader().getUniformId("uCenter");

    transition_out_buffer = new Bloom::Framebuffer(GetWindowSize().x, GetWindowSize().y, false);
    transition_out_buffer->setShader("res/app_boxer/shaders/framebuffer_transition_out.glsl");
    transition_out_radius_uid = transition_out_buffer->GetShader().getUniformId("uRadius");
    transition_out_center_uid = transition_out_buffer->GetShader().getUniformId("uCenter");
    transition_out_color_uid = transition_out_buffer->GetShader().getUniformId("uColor");

    transition_in_buffer = new Bloom::Framebuffer(GetWindowSize().x, GetWindowSize().y, false);
    transition_in_buffer->setShader("res/app_boxer/shaders/framebuffer_transition_in.glsl");
    transition_in_radius_uid = transition_in_buffer->GetShader().getUniformId("uRadius");
    transition_in_center_uid = transition_in_buffer->GetShader().getUniformId("uCenter");
    transition_in_color_uid = transition_in_buffer->GetShader().getUniformId("uColor");

    sound_hit = new SoLoud::Wav;
    sound_lose = new SoLoud::Wav;
    sound_health = new SoLoud::Wav;
    sound_radius = new SoLoud::Wav;
    sound_explosion = new SoLoud::Wav;
    sound_menu = new SoLoud::Wav;
    sound_music = new SoLoud::Wav;

    sound_hit->load("res/app_boxer/sounds/hit.wav");
    sound_lose->load("res/app_boxer/sounds/lose.wav");
    sound_health->load("res/app_boxer/sounds/health.wav");
    sound_radius->load("res/app_boxer/sounds/radius.wav");
    sound_explosion->load("res/app_boxer/sounds/explosion.wav");
    sound_menu->load("res/app_boxer/sounds/menu.wav");
    sound_music->load("res/app_boxer/sounds/music.wav");

    enemy_thread = new std::thread(&Boxer::ProcessEnemies, this);
    upgrade_thread = new std::thread(&Boxer::ProcessUpgrades, this);

    emitter_hit = new Bloom::ParticleEmitter<Bloom::Particle_HIT>(5,GetViewMatrix(),GetPixelProjectionMatrix());
    emitter_death = new Bloom::ParticleEmitter<Bloom::Particle_DEATH>(10, GetViewMatrix(), GetPixelProjectionMatrix());
    emitter_hurt = new Bloom::ParticleEmitter<Bloom::Particle_HURT>(50, GetViewMatrix(), GetPixelProjectionMatrix());
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


    if (!in_menu) {
        timer->Print(glm::vec2(50, 50), std::to_string((int)GetTime()), 1, GetWindowSize(), ((int)GetTime() % 10 == 0 && GetTime() >= 10) ? glm::vec4(0.15, 0.15, 0.15, 1) : glm::vec4(0.15, 0.15, 0.15, 1));
        timer->Print(glm::vec2(10, GetWindowSize().y - 25), ("FPS:" + std::to_string(GetFPS())).c_str(), 0.25, GetWindowSize(), glm::vec4(0.15, 0.15, 0.15, 1));
        timer->Print(glm::vec2(10, GetWindowSize().y - 50), ("VEL:" + std::to_string(player->GetSpeed())).c_str(), 0.25, GetWindowSize(), glm::vec4(0.15, 0.15, 0.15, 1));
        timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 2), (std::to_string(health)).c_str(), 0.5, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);
    }
    
    if (health <= 0 ) {
        in_menu = GetTime() >= transition_out_start_time + transition_out_duration;
        if (in_menu)
            StopSound(sound_music_id);
        else
            FadeVolume(sound_music_id, 0, transition_out_duration);

        transition_out_buffer->use(true);
        timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 1.5), "GAME OVER", 1.5, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);
        timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 3), "SCORE : " + std::to_string((int)transition_out_start_time), 0.5, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);
        timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 5), "PRESS ESC TO QUIT", 0.33, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);

        if ((int)GetTime() % 2 == 0 && GetTime() > transition_out_start_time + transition_out_duration)
            timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 2), "PRESS SPACE TO TRY AGAIN", 0.33, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);
        
        ProcessDeathMenu();
    }
    if(health > 0)
    {
        if (start_menu) {
            transition_in_buffer->use(true);
            timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 1.5), "TEST GAME", 1.5, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);
            timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 5), "PRESS ESC TO QUIT", 0.33, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);

            if ((int)GetTime() % 2 == 0 && GetTime() > transition_out_start_time + transition_out_duration)
                timer->Print(glm::vec2(GetWindowSize().x / 2, GetWindowSize().y / 2), "PRESS SPACE TO START", 0.33, GetWindowSize(), glm::vec4(0.21, 0.21, 0.21, 1), true);
        }
        else {
            transition_in_buffer->use(true);
            glClear(GL_COLOR_BUFFER_BIT);
        }


        if (GetTime() > transition_in_start_time + transition_in_duration + 1 && transition_in_start_time != -1) {
            start_menu = false;
        }
            

        ProcessStartMenu();
    }
}

void Boxer::GameLoop() {
    EnablePixels();

    DrawBackground();
    ProcessCamera();

    if (!in_menu) {
        DrawProtect();
        DrawPlayer();

        ProcessWaveEffect();
        wave_buffer->use(true);
        DrawEnemies();

        EnablePixels(false);
        wave_buffer->draw();

        if (ShouldSpawnEnemies()) {
            SpawnEnemies(EnemySpawnAmount());
        }

        DrawUpgrades();

        UpdateEmitters();

    }
    DrawPixelsToDefault();
}

void Boxer::InputLoop() {
    
}

void Boxer::DrawBackground() {
    move_circle_buffer->GetShader().use();
    move_circle_buffer->GetShader().uniformVec3(circle_data_uid, glm::vec3(GetPixelWindowSize().x, GetPixelWindowSize().y, move_radius));
    move_circle_buffer->GetShader().uniformVec2(circle_position_uid, player->GetPosition());
    move_circle_buffer->GetShader().uniformFloat(circle_time_uid, GetTime());
    move_circle_buffer->draw();
}

void Boxer::DrawPlayer() {
    player->SetMoveRadius(move_radius);

    if(GetTime() > 1)
        player->SetPosition(this);

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

    EraseEnemies();

    for (int i = 0; i < amount; i++)
    {
        int spawn_radius = GetPixelWindowSize().x * (1 + (float)i / amount) + rand() % variation;
        int angle = rand() % 360;
        glm::vec2 position = glm::vec2(
            (spawn_radius * cos(angle) + GetPixelWindowSize().x / 2),
            (spawn_radius * sin(angle) + GetPixelWindowSize().x / 2));

        enemies.push_back(new Application::Enemy(this, position));

        if (upgrade_health == nullptr && GetTime() >= 20) {
            spawn_radius = 72 + rand() % variation;
            angle = rand() % 360;
            position = glm::vec2(
                (spawn_radius * cos(angle) + GetPixelWindowSize().x / 2),
                (spawn_radius * sin(angle) + GetPixelWindowSize().x / 2));

            upgrade_health = new Application::Upgrade_Health(this, position);
        }

        if (last_hp <= health && upgrade_radius == nullptr && GetTime() >= 20) {
            spawn_radius = 72 + rand() % variation;
            angle = rand() % 360;
            position = glm::vec2(
                (spawn_radius * cos(angle) + GetPixelWindowSize().x / 2),
                (spawn_radius * sin(angle) + GetPixelWindowSize().x / 2));

            upgrade_radius = new Application::Upgrade_Radius(this, position);
        }
    }

    spawned_wave = true;
}

void Boxer::ProcessEnemies() {
    while (true) {
        Utils::SyncSleep(this);

        for (size_t i = 0; i < enemies.size(); i++) {
            enemies.at(i)->SetPosition(this);
            if (Utils::Distance2D(enemies.at(i)->GetPosition(), glm::vec2(GetPixelWindowSize().x / 2, GetPixelWindowSize().y / 2)) < 10) {
                Damage();
                enemies.erase(enemies.begin() + i);
            }

            if (enemies.size() > i) {
                if (enemies.at(i)->GetCollisionBox()->check(*player->GetCollisionBox())) {
                    if (enemies.at(i) != nullptr) {
                        enemies.at(i)->SetVelocity(enemies.at(i)->GetVelocity() + (player->GetVelocity() * glm::vec2(0.6)));
                        PlaySound(sound_hit, Utils::RandFloat(0.7, 1.3));
                        PlayEmitterHIT();
                    }
                }
            }
        }
    }
}

void Boxer::DrawEnemies() {
    for (size_t i = 0; i < enemies.size(); i++) {
        enemies.at(i)->Draw(this);
    }
}

void Boxer::Damage() {
    if (health > 0) {
        PlaySound(sound_explosion, Utils::RandFloat(0.9, 1.1));
        health -= 1;
        PlayEmitterHURT();

        if (health <= 0 && !in_menu) {
            PlaySound(sound_lose);
            upgrade_health == nullptr;
            transition_out_start_time = GetTime();
        }
    }
    
    hitShakeY = new Bloom::Shake(12,4, 0.25,3);
    hitShakeY->Start(GetTime());
    hitShakeX = new Bloom::Shake(12, 4, 0.25,3);
    hitShakeX->Start(GetTime());
}

void Boxer::DrawProtect() {
    float rotation = GetTime() * 36;
    protect->SetRotation(rotation);

    protect->GetShader()->use();
    protect->GetShader()->uniformVec4(protect_color_uid,Utils::Lerp(glm::vec4(0.9, 0.97, 1, 1),glm::vec4(0, 0.75, 1,1),(health/3.0)));
    protect->Draw(GetViewMatrix());
}

void Boxer::ProcessCamera() {
    if (hitShakeY != nullptr && hitShakeX != nullptr) {
        camera_location.y = hitShakeY->GetValue(GetTime());
        camera_location.x = hitShakeX->GetValue(GetTime());
    }
    SetCameraLocation(camera_location);
}

void Boxer::DrawUpgrades() {
    if (upgrade_health != nullptr) {
        upgrade_health->Draw(this);
    }

    if (upgrade_radius!= nullptr) {
        upgrade_radius->Draw(this);
    }
}

void Boxer::ProcessUpgrades() {
    while (true) {
        Utils::SyncSleep(this);

        if (upgrade_health != nullptr) {
            upgrade_health->SetPosition(this);
            if (Utils::Distance2D(upgrade_health->GetPosition(), glm::vec2(GetPixelWindowSize().x / 2, GetPixelWindowSize().y / 2)) < 10 && health > 0) {
                last_hp = health;
                health += 1;
                upgrade_health = nullptr;
                PlaySound(sound_health);
            }
        }

        if (upgrade_radius != nullptr) {
            upgrade_radius->SetPosition(this);
            if (Utils::Distance2D(upgrade_radius->GetPosition(), glm::vec2(GetPixelWindowSize().x / 2, GetPixelWindowSize().y / 2)) < 10) {
                move_radius += 5;
                upgrade_radius = nullptr;
                PlaySound(sound_radius);
            }
        }
    }
}

void Boxer::ProcessWaveEffect() {
    float time = ((GetTime()) - (int)GetTime()) + (int)GetTime() % 10;
    float start = 9.75; float duration = 0.25;
    float radius = Utils::Lerp(0.0, (std::sqrt(2) * std::max(GetPixelWindowSize().x, GetPixelWindowSize().y) * 0.5), (time - start) / duration);
    wave_buffer->GetShader().use();
    wave_buffer->GetShader().uniformVec2(wave_center_uid, GetPixelWindowSize() / glm::vec2(2));
    wave_buffer->GetShader().uniformFloat(wave_radius_uid,radius);
    if(radius > 10)
        for (int i = 0; i < enemies.size(); i++)
        {
            if (Utils::Distance2D(enemies.at(i)->GetPosition(), GetPixelWindowSize() / glm::vec2(2)) <= radius) {
                PlayEmitterDEATH(enemies.at(i));
                enemies.erase(enemies.begin() + i);
            }
        }
    
}

void Boxer::ProcessDeathMenu() {
    transition_out_buffer->GetShader().use();
    transition_out_buffer->GetShader().uniformVec2(transition_out_center_uid, GetWindowSize() / glm::vec2(2));
    transition_out_buffer->GetShader().uniformFloat(transition_out_radius_uid, Utils::Lerp(0.0, (std::sqrt(2) * std::max(GetWindowSize().x, GetWindowSize().y) * 0.5), (GetTime()- transition_out_start_time) / (transition_out_duration)));
    transition_out_buffer->GetShader().uniformVec3(transition_out_color_uid, glm::vec3(Utils::Lerp(glm::vec4(0.9, 0.97, 1, 1), glm::vec4(0.1, 0.775, 1, 1), (health / 3.0))));

    bindDefaultFramebuffer(false);
    transition_out_buffer->draw();
}

void Boxer::ProcessStartMenu() {
    transition_in_buffer->GetShader().use();
    transition_in_buffer->GetShader().uniformVec2(transition_in_center_uid, GetWindowSize() / glm::vec2(2));
    if (transition_in_start_time == -1)
        transition_in_buffer->GetShader().uniformFloat(transition_in_radius_uid, 0);
    else
        transition_in_buffer->GetShader().uniformFloat(transition_in_radius_uid, Utils::Lerp(0.0, (std::sqrt(2) * std::max(GetWindowSize().x, GetWindowSize().y) * 0.5), GetTime() / (transition_in_duration)));

    if(start_menu)
        transition_in_buffer->GetShader().uniformVec3(transition_in_color_uid, glm::vec3(Utils::Lerp(glm::vec4(0.9, 0.97, 1, 1), glm::vec4(0.1, 0.775, 1, 1), (health / 3.0))));
    else
        transition_in_buffer->GetShader().uniformVec3(transition_in_color_uid, glm::vec3(0.9, 0.97, 1));

    bindDefaultFramebuffer(false);
    transition_in_buffer->draw();
}

void Boxer::EraseEnemies() {
    for (int i = 0; i < enemies.size(); i++)
    {
        enemies.erase(enemies.begin());
    }
}

void Boxer::UpdateEmitters() {
    emitter_hit->Update(GetTime(), GetGameSpeed());
    emitter_hit->Draw();
    emitter_death->Update(GetTime(), GetGameSpeed());
    emitter_death->Draw();
    emitter_hurt->Update(GetTime(), GetGameSpeed());
    emitter_hurt->Draw();
}