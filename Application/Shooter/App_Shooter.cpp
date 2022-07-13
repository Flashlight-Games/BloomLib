#include "App_Shooter.h"

using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = Utils::GetApp<Application::Shooter>(window);
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            glfwSetWindowShouldClose(app->GetWindow(), true);
        }
        break;
    case GLFW_KEY_P:
        if (action == GLFW_PRESS) {
            app->SearchLobbies();
        }
        break;
    }
}

void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    auto app = Utils::GetApp<Application::Shooter>(window);
    switch (key) {
    case GLFW_MOUSE_BUTTON_LEFT:
        if (action == GLFW_PRESS) {
            app->GetChecker()->SetPosition(app->GetCursorPosition(), app->GetWindow(), app->GetTime(), app->GetmainBox());
        }
    }
}

void Shooter::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);

    text_pressstart = new Bloom::Text("res/engine/fonts/PressStart/PressStart.ttf",GetViewMatrix(),GetWindowProjectionMatrix(),-0.5,128);

    sprite_block = new Bloom::Sprite("res/engine/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5));
    sprite_block->SetLocation(GetPixelWindowSize() / glm::vec2(2));
    sprite_block->SetRotation(0);
    sprite_block->SetScale(glm::vec2(1));

    sprite_block->GetShader()->use();
    block_color_uid = sprite_block->GetShader()->getUniformId("uColor");

    mainBox = new PhysX::collisionBox("main", sprite_block);

    checker = new Application::Checker(25,GetViewMatrix(),GetPixelProjectionMatrix());
}

void Shooter::UILoop() {
    ImGui::Begin("Control window");
    ImGui::ColorPicker3("BLOCK COLOR", block_color);
    text_pressstart->GetTexture()->ImGui(0.1);
    ImGui::End();

    if (Utils::isDevelopmentConfiguration()) {
        ImGui::Begin("Performance window");
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
        ImGui::End();
    }
        
    text_pressstart->Print(glm::vec2(10, GetWindowSize().y - 25), ("FPS:" + std::to_string(GetFPS())).c_str(), 0.25, GetWindowSize(), glm::vec4(1, 1, 1, 1));
}

void Shooter::GameLoop() {
    //NETWORKING
    switch (network_status) {
    case SteamTools::NetworkStatus::offline:
        status_string = "OFFLINE";
        break;
    case SteamTools::NetworkStatus::searching:
        status_string = "SEARCHING";
        if (GetSteamManager()->GetLobbiesFound() != -1) {
            if (GetSteamManager()->GetLobbiesFound() == 0) {
            }
            else {
                for (int i = 0; i < GetSteamManager()->GetLobbiesFound() && network_status == SteamTools::NetworkStatus::searching; i++)
                {
                    std::string bloom_game_name = SteamMatchmaking()->GetLobbyData(SteamMatchmaking()->GetLobbyByIndex(i), "BloomGameName");
                    std::cout << "FOUND \"" << bloom_game_name << "\"\n";
                    if(bloom_game_name == "Shooter")
                        current_lobby = JoinLobby(i);
                }
                if (network_status == SteamTools::NetworkStatus::searching) {
                    GetSteamManager()->CreateLobby(8);
                    SetNetworkStatus(SteamTools::NetworkStatus::creating);
                }
            }
        }
        break;
    case SteamTools::NetworkStatus::joining:
        status_string = "JOINING";
        if (GetSteamManager()->GetLobbyConnectionSuccess())
            SetNetworkStatus(SteamTools::NetworkStatus::connected);
        break;
    case SteamTools::NetworkStatus::creating:
        status_string = "CREATING";
        if (GetSteamManager()->GetCreatedLobbyId() != static_cast<uint64>(-1)) {
            if (GetSteamManager()->GetCreatedLobbyId() == static_cast<uint64>(0))
                SetNetworkStatus(SteamTools::NetworkStatus::offline);
            else {
                current_lobby = GetSteamManager()->GetCreatedLobbyId();
                if (SteamMatchmaking()->SetLobbyData(current_lobby, "BloomGameName", "Shooter"))
                    std::cout << "SUCCESS!\n";
                SetNetworkStatus(SteamTools::NetworkStatus::connected);
            }
        }
        break;
    case SteamTools::NetworkStatus::connected:
        status_string = "CONNECTED/" + std::to_string(SteamMatchmaking()->GetNumLobbyMembers(current_lobby));
        break;
    }
    

    //RENDERING
    text_pressstart->Print(GetWindowSize() / glm::vec2(2) + glm::vec2(0, -150), status_string, 1, GetWindowSize(), glm::vec4(1), true);
    text_pressstart->Print(GetWindowSize() / glm::vec2(2) + glm::vec2(0, 150), SteamTools::SteamManager::GetMySteamName(), 1, GetWindowSize(), glm::vec4(1), true);
    

    EnablePixels();

    sprite_block->GetShader()->use();
    sprite_block->GetShader()->uniformVec4(block_color_uid, glm::vec4(block_color[0], block_color[1], block_color[2], 1));
    sprite_block->SetRotation((GetTime() / 10.0) * 360);
    sprite_block->Draw(GetViewMatrix());

    mainBox->setLocation(sprite_block->GetLocation());
    mainBox->setRotation(sprite_block->GetRotation());
    mainBox->setScale(sprite_block->GetSize());

    checker->Draw(GetTime(), GetGameSpeed());

    DrawPixelsToDefault(false);
}

void Shooter::InputLoop() {
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_SPACE)) {
        auto newLocation = GetPixelWindowSize() / glm::vec2(2) + glm::vec2(0, 50 * Utils::Noise_Simplex(glm::vec2(0, GetTime())) / 2);
        GetSptiteBlock()->SetLocation(newLocation);
    }
}