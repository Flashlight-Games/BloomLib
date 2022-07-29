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
            app->GetSteamManager()->FindLobbies();
        }
        break;
    case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS) {
            if (app->GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::host || app->GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::remote) {
                auto newLocation = app->GetPixelWindowSize() / glm::vec2(2) + glm::vec2(0, 50 * Utils::Noise_Simplex(glm::vec2(0, app->GetTime())) / 2);
                app->GetSteamManager()->SendMessageP2P_Reliable(0, std::to_string(key).c_str(), SteamTools::DataType::PLAYER_INPUT_KEY_PRESS, std::vector<std::string>({ std::to_string(newLocation.y) })); // SEND PRESSED SPACE AND NEW Y
            }
            else {
                app->GetCharacter()->Jump();
            }
        }
    }
}

void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    auto app = Utils::GetApp<Application::Shooter>(window);
    switch (key) {
    case GLFW_MOUSE_BUTTON_LEFT:
        if (action == GLFW_PRESS) {
            glm::vec2 position = app->GetCursorPosition() - (app->GetPixelWindowSize() / glm::vec2(2));
            position = (position + app->GetCharacter()->GetLocation()) / glm::vec2(16);
            app->GetCave()->SetBlock(glm::round(position), cave_tile::empty);
        }
    }
}

void Shooter::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);

    text_pressstart = new Bloom::Text("res/engine/fonts/PressStart/PressStart.ttf",GetViewMatrix(),GetWindowProjectionMatrix(),-0.5,128);

    cave_generator = new Application::Cave(GetViewMatrix(), GetPixelProjectionMatrix(), -1);
    player_character = new Application::Player(GetViewMatrix(), GetPixelProjectionMatrix());

    collisionA = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionB = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionC = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionD = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionE = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionF = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionG = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionH = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));
    collisionI = new PhysX::collisionBox("main", glm::vec2(0), glm::vec2(16));

    update_thread = new std::thread(&Application::Shooter::Update, this);
}

void Shooter::UILoop() {
    ImGui::Begin("Control window"); {
        //ImGui::ColorPicker3("BLOCK COLOR", block_color);
        if (Utils::isDevelopmentConfiguration()) {
            ImGui::TextColored(ImVec4(0, 1, 0.5, 1), (std::to_string(player_character->GetLocation().x).c_str()));
            ImGui::TextColored(ImVec4(0, 1, 0.5, 1), (std::to_string(player_character->GetLocation().y).c_str()));
            ImGui::TextColored(ImVec4(0, 1, 1, 1), (std::to_string(player_character->getvel()).c_str()));
        }
        ImGui::TextColored(ImVec4(1, 0.2,0.2, 1), (std::string("GAMEPAD : ").c_str() + std::to_string(jid)).c_str());
        ImGui::TextColored(ImVec4(1, 0.5, 0.0, 1), (std::string("GLFW v_").c_str() + std::string(glfwGetVersionString())).c_str());
    } ImGui::End();

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
    if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::local) {
        GetSteamManager()->MatchmakeLoop_Quick(4,GetTime()); // UPDATE MATCHMAKING NETWORK STATUS
    } if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::lobby) {
        GetSteamManager()->StartWhenReady(GetTime(), 0, true, 2, 120);
    } if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::remote) {
        GetSteamManager()->ReceiveMessageP2P();
        ParseNetworkMessage(GetSteamManager()->GetReceivedMessage());
    } if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::host) {
        GetSteamManager()->ReceiveMessageP2P();
        ParseNetworkMessage(GetSteamManager()->GetReceivedMessage());
    }


    SetStatusString(GetSteamManager()->GetNetworkStatus(), GetSteamManager()->GetNetworkPosition());

    //RENDERING
    text_pressstart->Print(GetWindowSize() / glm::vec2(2) + glm::vec2(0, -150), status_string, 1, GetWindowSize(), glm::vec4(1), true);
    text_pressstart->Print(GetWindowSize() / glm::vec2(2) + glm::vec2(0, 150), SteamTools::SteamManager::GetMySteamName(), 1, GetWindowSize(), glm::vec4(1), true);

    if (GetSteamManager()->GetNetworkStatus() == SteamTools::NetworkStatus::connected) {
        auto LobbyNames = GetSteamManager()->GetLobbyMemberNames();
        for (size_t i = 0; i < LobbyNames.size(); i++)
        {
            text_pressstart->Print(glm::vec2(3, 20 * i+20), LobbyNames.at(i), 0.25, GetWindowSize(), glm::vec4(1));
        }
    }
    if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::lobby)
        text_pressstart->Print(glm::vec2(3, 0), std::to_string(GetSteamManager()->GetCurrentLobby().GetAccountID()), 0.25, GetWindowSize(), glm::vec4(1));
    else if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::remote)
        text_pressstart->Print(glm::vec2(3, 0), "REMOTE", 0.25, GetWindowSize(), glm::vec4(1));
    else if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::host)
        text_pressstart->Print(glm::vec2(3, 0), "HOST", 0.25, GetWindowSize(), glm::vec4(1));

    EnablePixels();

    player_character->Draw(GetViewMatrix());
    cave_generator->Draw(GetViewMatrix(), player_character->GetLocation(), GetPixelWindowSize());

    DrawPixelsToDefault(false);
}

void Shooter::InputLoop() {
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_SPACE)) {
        /*auto newLocation = GetPixelWindowSize() / glm::vec2(2) + glm::vec2(0, 50 * Utils::Noise_Simplex(glm::vec2(0, GetTime())) / 2);
        GetSptiteBlock()->SetLocation(newLocation);*/
    }
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_RIGHT))
        this->character_input.x = 1;
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_LEFT))
        this->character_input.x = -1;
    if (!Utils::isKeyPressed(GetWindow(), GLFW_KEY_LEFT) && !Utils::isKeyPressed(GetWindow(), GLFW_KEY_RIGHT))
        this->character_input.x = 0.0;


    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_UP))
        this->character_input.y = 1;
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_DOWN))
        this->character_input.y = -1;
    if (!Utils::isKeyPressed(GetWindow(), GLFW_KEY_UP) && !Utils::isKeyPressed(GetWindow(), GLFW_KEY_DOWN))
        this->character_input.y = 0.0;

    InputHandle_t handlesOut[STEAM_INPUT_MAX_COUNT];
    int found =  SteamInput()->GetConnectedControllers(handlesOut);
    if (found > 0) {
        auto LeftData = SteamInput()->GetAnalogActionData(handlesOut[0], SteamInput()->GetAnalogActionHandle("analog_controls"));
        this->character_input.x = LeftData.x * 1;
        this->character_input.y = LeftData.y * 1;

        auto FireData = SteamInput()->GetDigitalActionData(handlesOut[0], SteamInput()->GetDigitalActionHandle("fire_lasers"));
        if (FireData.bState)
            character_input *= glm::vec2(3);
    }

    player_character->Move(character_input);
}

void Application::Shooter::SetStatusString(SteamTools::NetworkStatus status, SteamTools::NetworkPosition position){
    switch (status) {
    case SteamTools::NetworkStatus::connected:
        status_string = "CONNECTED";
        break;
    case SteamTools::NetworkStatus::offline:
        status_string = "OFFLINE";
        break;
    case SteamTools::NetworkStatus::creating:
        status_string = "CREATING";
        break;
    case SteamTools::NetworkStatus::joining:
        status_string = "JOINING";
        break;
    case SteamTools::NetworkStatus::searching:
        status_string = "SEARCHING";
        break;
    }

    switch (position) {
    case SteamTools::NetworkPosition::local:
        status_string = "LC_" + status_string;
        break;
    case SteamTools::NetworkPosition::lobby:
        status_string = "LB_" + status_string + '/' + std::to_string(SteamMatchmaking()->GetNumLobbyMembers(GetSteamManager()->GetCurrentLobby()));
        break;
    case SteamTools::NetworkPosition::remote:
        status_string = "RE_" + status_string;
        break;
    case SteamTools::NetworkPosition::host:
        status_string = "SE_" + status_string + '/' + std::to_string(GetSteamManager()->GetRemotes().size());
        break;
    }
}

void Application::Shooter::ParseNetworkMessage(SteamTools::Message message)
{
    switch (message.type) {
    case SteamTools::DataType::TIME_SYNC:
        glfwSetTime(stoi(message.data));
        break;
    case SteamTools::DataType::PLAYER_INPUT_KEY_PRESS:
        if (std::stoi(message.data) == GLFW_KEY_SPACE) {
            
        }
        break;
    }
}

void Application::Shooter::Update() {
    while (true) {
        Utils::SyncSleep(300);
        last_update_time = current_update_time;
        current_update_time = GetHighResTime();
        std::chrono::milliseconds delta = std::chrono::duration_cast<std::chrono::milliseconds>(current_update_time - last_update_time);

        collisionA->setLocation(glm::vec2(-32));
        collisionB->setLocation(glm::vec2(-32));
        collisionC->setLocation(glm::vec2(-32));
        collisionD->setLocation(glm::vec2(-32));
        collisionE->setLocation(glm::vec2(-32));
        collisionF->setLocation(glm::vec2(-32));
        collisionG->setLocation(glm::vec2(-32));
        collisionH->setLocation(glm::vec2(-32));
        collisionI->setLocation(glm::vec2(-32));

        auto tiles = cave_generator->GetTiles();
        glm::vec2 playerpos = glm::round(player_character->GetLocation() / glm::vec2(16));

        playerpos.x = Utils::clamp(playerpos.x, 1, tiles.size() - 1);
        playerpos.y = Utils::clamp(playerpos.y, 1, tiles.at(playerpos.x).size() - 1);
        if (tiles.at(playerpos.x + 1).at(playerpos.y) == cave_tile::block)
            collisionA->setLocation((playerpos + glm::vec2(1, 0)) * glm::vec2(16));
        if (tiles.at(playerpos.x - 1).at(playerpos.y) == cave_tile::block)
            collisionB->setLocation((playerpos + glm::vec2(-1, 0)) * glm::vec2(16));
        if (tiles.at(playerpos.x).at(playerpos.y + 1) == cave_tile::block)
            collisionC->setLocation((playerpos + glm::vec2(0, 1)) * glm::vec2(16));
        if (tiles.at(playerpos.x).at(playerpos.y - 1) == cave_tile::block)
            collisionD->setLocation((playerpos + glm::vec2(0, -1)) * glm::vec2(16));
        if (tiles.at(playerpos.x + 1).at(playerpos.y + 1) == cave_tile::block)
            collisionE->setLocation((playerpos + glm::vec2(1, 1)) * glm::vec2(16));
        if (tiles.at(playerpos.x - 1).at(playerpos.y + 1) == cave_tile::block)
            collisionF->setLocation((playerpos + glm::vec2(-1, 1)) * glm::vec2(16));
        if (tiles.at(playerpos.x + 1).at(playerpos.y - 1) == cave_tile::block)
            collisionG->setLocation((playerpos + glm::vec2(1, -1)) * glm::vec2(16));
        if (tiles.at(playerpos.x - 1).at(playerpos.y - 1) == cave_tile::block)
            collisionH->setLocation((playerpos + glm::vec2(-1, -1)) * glm::vec2(16));
        if (tiles.at(playerpos.x).at(playerpos.y) == cave_tile::block)
            collisionI->setLocation((playerpos + glm::vec2(0, 0)) * glm::vec2(16));

        player_character->Update(delta.count(), std::vector<PhysX::collisionBox*>({ collisionA,collisionB,collisionC,collisionD,collisionE,collisionF,collisionG,collisionH ,collisionI }));
        camera_location = -(player_character->GetLocation() - (GetPixelWindowSize() / glm::vec2(2)));
        SetCameraLocation(camera_location);
    }
}