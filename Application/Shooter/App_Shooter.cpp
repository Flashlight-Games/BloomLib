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
            if (app->GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::local) {
                app->ResetPoints();
                app->GetSteamManager()->FindLobbies();
            }
            else {
                app->GetSteamManager()->Disconnect();
            }
        }
        break;

    case GLFW_KEY_LEFT_SHIFT:
        if (app->GetStarted()) {
            if (action == GLFW_PRESS) {
                app->ActivateAbility();
            }
            else if (action == GLFW_RELEASE) {
                app->DeactivateAbility();
            }
            app->GetSteamManager()->SendMessageP2P_Reliable(0, std::to_string(GLFW_KEY_LEFT_SHIFT).c_str(), SteamTools::DataType::PLAYER_INPUT_STATE_PRESSED, std::vector<std::string>({std::to_string(action == GLFW_PRESS)})); // SEND {LSHIFT} OTHER ABILITY
            app->GetSteamManager()->SendMessageP2P_Reliable(0, "6", SteamTools::DataType::GAME_STATE, std::vector<std::string>({ std::to_string(app->GetCursorWorldPosition().x),std::to_string(app->GetCursorWorldPosition().y) })); // SEND CURSOR LOCATION
        }
        break;

    case GLFW_KEY_SPACE:
        if (app->GetStarted()) {
            if (action == GLFW_PRESS) {
                app->GetCharacter()->Jump();
                app->GetSteamManager()->SendMessageP2P_Reliable(0, std::to_string(GLFW_KEY_SPACE).c_str(), SteamTools::DataType::PLAYER_INPUT_KEY_PRESS, std::vector<std::string>({})); // SEND {SPACE} OTHER JUMP
            }
            else {
                app->GetCharacter()->ResetJump();
                app->GetSteamManager()->SendMessageP2P_Reliable(0, std::to_string(GLFW_KEY_SPACE).c_str(), SteamTools::DataType::PLAYER_INPUT_KEY_RELEASE, std::vector<std::string>({})); // SEND {SPACE} OTHER JUMP RELEASE
            }
        }
        break;


    case GLFW_KEY_A:
        app->GetSteamManager()->SendMessageP2P_Reliable(0, "1", SteamTools::DataType::GAME_STATE, std::vector<std::string>({
            std::to_string(app->GetCharacter()->GetLocation().x),std::to_string(app->GetCharacter()->GetLocation().y)})); // SEND {1,LOCX,LOCY} PLAYER POSITION
        break;

    case GLFW_KEY_D:
        app->GetSteamManager()->SendMessageP2P_Reliable(0, "1", SteamTools::DataType::GAME_STATE, std::vector<std::string>({
            std::to_string(app->GetCharacter()->GetLocation().x),std::to_string(app->GetCharacter()->GetLocation().y) })); // SEND {1,LOCX,LOCY} PLAYER POSITION
        break;
    }
}

void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    auto app = Utils::GetApp<Application::Shooter>(window);
    switch (key) {
    case GLFW_MOUSE_BUTTON_LEFT:
        if (app->GetStarted()) {
            app->SetFiring(action == GLFW_PRESS);
        }
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        if (app->GetStarted()) {
            if (action == GLFW_PRESS) {
                //glm::vec2 position = app->GetCursorPosition() - (app->GetPixelWindowSize() / glm::vec2(2));
                //position = glm::round((position + app->GetCharacter()->GetLocation()) / glm::vec2(16));
                //app->GetCave()->SetBlock(position, cave_tile::block);

                //app->GetSteamManager()->SendMessageP2P_Reliable(0, std::to_string(GLFW_MOUSE_BUTTON_RIGHT).c_str(), SteamTools::DataType::PLAYER_INPUT_KEY_PRESS, std::vector<std::string>({ std::to_string(position.x),std::to_string(position.y) })); // SEND {RIGHT_CLICK,X,Y}  PLACE
                app->PlayerZoom();
            }
            else if (action == GLFW_RELEASE) {
                app->PlayerStopZoom();
            }
        }
        break;
    }
}

void Shooter::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);

    text_pressstart = new Bloom::Text("res/engine/fonts/PressStart/PressStart.ttf",GetViewMatrix(),GetWindowProjectionMatrix(),-0.5,128);

    player_character = new Application::Player(GetViewMatrix(), GetPixelProjectionMatrix());
    other_character = new Application::Player(GetViewMatrix(), GetPixelProjectionMatrix());
    cave_generator = new Application::Cave(GetViewMatrix(), GetPixelProjectionMatrix());

    player_compass = new Compass(player_character,other_character,GetViewMatrix(),GetPixelProjectionMatrix() );

    player_character->SetHue(Utils::RandInt(0, 360));

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
            ImGui::TextColored(ImVec4(1, 0.2, 0.2, 1), (std::string("GAMEPAD : ").c_str() + std::to_string(jid)).c_str());
            ImGui::TextColored(ImVec4(1, 0.5, 0.0, 1), (std::string("GLFW v_").c_str() + std::string(glfwGetVersionString())).c_str());
        }
        ImGui::TextColored(ImVec4(0, 1, 0, 1), ((std::string("PLAYER HP: ") + std::to_string(player_character->GetHealth())).c_str()));
        ImGui::TextColored(ImVec4(0, 1, 0, 1), ((std::string("ENEMY HP: ") + std::to_string(other_character->GetHealth())).c_str()));
        ImGui::TextColored(ImVec4(0, 1, 0.5, 1), (std::to_string(player_character->GetLocation().x).c_str()));
        ImGui::TextColored(ImVec4(0, 1, 0.5, 1), (std::to_string(player_character->GetLocation().y).c_str()));
        ImGui::TextColored(ImVec4(0, 1, 1, 1), (std::string("MY POINTS: ") + std::to_string(my_points)).c_str());
        ImGui::TextColored(ImVec4(0, 1, 1, 1), (std::string("OTHER POINTS: ") + std::to_string(other_points)).c_str());
    } ImGui::End();

    if (Utils::isDevelopmentConfiguration()) {
        ImGui::Begin("Performance window");
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
        ImGui::SliderInt("PlayerPoints", &my_points, 0, 3);
        ImGui::SliderInt("OtherPoints", &other_points, 0, 3);
        ImGui::End();
    }
        
    text_pressstart->Print(glm::vec2(10, GetWindowSize().y - 25), ("FPS:" + std::to_string(GetFPS())).c_str(), 0.25, GetWindowSize(), glm::vec4(1, 1, 1, 1));
    if (my_points >= 3 || other_points >= 3) {
        text_pressstart->Print(GetWindowSize() / glm::vec2(2), (my_points > other_points) ? "VICTORY" : "DEFEAT", 2, GetWindowSize(), glm::vec4(1, 1, 1, 1), true);
    }
}

void Shooter::GameLoop() {
    //NETWORKING
    if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::local) {
        started = false;
        GetSteamManager()->MatchmakeLoop_Quick(2,GetTime()); // UPDATE MATCHMAKING NETWORK STATUS
    } if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::lobby) {
        GetSteamManager()->StartWhenReady(GetTime(), 0, true, 2, 120);
    } if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::remote) {
        GetSteamManager()->ReceiveMessageP2P();
        ParseNetworkMessage(GetSteamManager()->GetReceivedMessage());
    } if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::host) {
        GetSteamManager()->ReceiveMessageP2P();
        ParseNetworkMessage(GetSteamManager()->GetReceivedMessage());
        if (!started) {
            if(GetSteamManager()->GetRemotes().size() > 0)
                Start();
        }
    }

    SetStatusString(GetSteamManager()->GetNetworkStatus(), GetSteamManager()->GetNetworkPosition());

    //RENDERING
    glm::vec2 player_camera_location = -(player_character->GetLocation() - (GetPixelWindowSize() / glm::vec2(2)));
    zoom_location = -((GetCursorPosition() - GetPixelWindowSize() / glm::vec2(2)) / (GetPixelWindowSize() / glm::vec2(2)) * glm::vec2(player_gun->GetZoom())) + player_camera_location;

    camera_location = ((Utils::Lerp(camera_location, (zoom) ? zoom_location : player_camera_location, 0.15)));
    SetCameraLocation(camera_location);

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

    other_character->Draw(GetViewMatrix());
    player_character->Draw(GetViewMatrix());
    cave_generator->Draw(GetViewMatrix(), player_character->GetLocation(), GetPixelWindowSize());

    player_compass->Update();
    player_compass->Draw(GetViewMatrix());

    DrawPixelsToDefault(false);
}

void Shooter::InputLoop() {

    last_input = character_input;

    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_D))
        this->character_input.x = 1;
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_A))
        this->character_input.x = -1;
    if (!Utils::isKeyPressed(GetWindow(), GLFW_KEY_A) && !Utils::isKeyPressed(GetWindow(), GLFW_KEY_D))
        this->character_input.x = 0.0;

    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_W))
        this->character_input.y = 1;
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_S))
        this->character_input.y = -1;
    if (!Utils::isKeyPressed(GetWindow(), GLFW_KEY_S) && !Utils::isKeyPressed(GetWindow(), GLFW_KEY_W))
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

    player_character->Move(GetDeltaTime(), character_input);
    player_gun->Aim(std::round(glm::degrees(std::atan2(player_character->GetLocation().y - GetCursorWorldPosition().y, player_character->GetLocation().x - GetCursorWorldPosition().x)) + 180));

    other_character->Move(GetDeltaTime(),other_input);

    if(character_input != last_input)
        GetSteamManager()->SendMessageP2P_Reliable(0, "0", SteamTools::DataType::PLAYER_INPUT_STATE_PRESSED, std::vector<std::string>({ std::to_string(character_input.x),std::to_string(character_input.y) })); // SEND {0,INPUTX} OTHER XINPUT

    if (firing) {
        if (PlayerFire()) {
            GetSteamManager()->SendMessageP2P_Reliable(0, std::to_string(GLFW_MOUSE_BUTTON_LEFT).c_str(), SteamTools::DataType::PLAYER_INPUT_KEY_PRESS, std::vector<std::string>({ std::to_string(GetPlayerAimAngle()),std::to_string(GetPlayerAmmo()),std::to_string(GetCharacter()->GetLocation().x),std::to_string(GetCharacter()->GetLocation().y) })); // SEND {LEFT_CLICK,X,Y}  DELETE
        }
    }
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
    last_msg = GetTime();
    switch (message.type) {
    case SteamTools::DataType::TIME_SYNC:
        glfwSetTime(stoi(message.data));
        break;
    case SteamTools::DataType::PLAYER_INPUT_KEY_PRESS:
        if (std::stoi(message.data) == GLFW_KEY_SPACE) {
            other_character->Jump();
        } else if (std::stoi(message.data) == GLFW_MOUSE_BUTTON_LEFT) {
            other_gun->SetLocation(glm::vec2(std::stof(message.flags.at(2)), std::stof(message.flags.at(3))));
            other_gun->Fire(GetTime(), cave_generator, player_character, glm::vec3(std::stof(message.flags.at(2)), std::stof(message.flags.at(3)), std::stof(message.flags.at(0))));
        } else if (std::stoi(message.data) == GLFW_MOUSE_BUTTON_RIGHT) {
            GetCave()->SetBlock(glm::vec2(std::stof(message.flags.at(0)), std::stof(message.flags.at(1))), cave_tile::block);
        }
        break;
    case SteamTools::DataType::PLAYER_INPUT_KEY_RELEASE:
        if (std::stoi(message.data) == GLFW_KEY_SPACE) {
            other_character->ResetJump();
        }
        break;
    case SteamTools::DataType::GAME_STATE:
        if (std::stoi(message.data) == 0) { // WORLD SEED
            Start(std::stoi(message.flags.at(0)));
            std::cout << "STARTED GAME ON SEED " << message.flags.at(0) << "\n";
        } else if (std::stoi(message.data) == 1) { // PLAYER POSITION
            other_character->SetLocation(glm::vec2(std::stof(message.flags.at(0)), std::stof(message.flags.at(1))));
        } else if (std::stoi(message.data) == 2) { // PLAYER COLOR
            other_character->SetHue(std::stoi(message.flags.at(0)));
        } else if (std::stoi(message.data) == 3) { // PLAYER VELOCITY
            other_character->SetVelocity(glm::vec2(std::stof(message.flags.at(0)), std::stof(message.flags.at(1))));
        } else if (std::stoi(message.data) == 4) { // OTHER PLAYER HEALTH
            other_character->SetHealth(std::stoi(message.flags.at(0)));
        } else if (std::stoi(message.data) == 5) { // POINT UPDATE
            my_points++;
            if (other_points < 3 && my_points < 3)
                StartRound();
        } else if (std::stoi(message.data) == 6) { // CURSOR UPDATE
            other_character->SetCursorPosition(glm::vec2(std::stof(message.flags.at(0)), std::stof(message.flags.at(1))));
        }
        break;
    case SteamTools::DataType::PLAYER_INPUT_STATE_PRESSED:
        if (std::stoi(message.data) == 0) { // XINPUT
            this->other_input.x = std::stof(message.flags.at(0)); 
            this->other_input.y = std::stof(message.flags.at(1));
        }
        else if (std::stoi(message.data) == GLFW_KEY_LEFT_SHIFT) {
            if (std::stoi(message.flags.at(0)) == GLFW_PRESS) {
                this->other_ability->Activate(GetTime());
            }
            else {
                this->other_ability->Deactivate(GetTime());
            }
        }
        break;
    default:
        //std::cout << "UNKOWN DATATYPE\n";
        break;
    }
}

void Application::Shooter::Update() {
    while (!glfwWindowShouldClose(GetWindow())) {
        if (started) {
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
            glm::vec2 otherplayerpos = glm::round(other_character->GetLocation() / glm::vec2(16));

            playerpos.x = Utils::clamp(playerpos.x, 1, tiles.size() - 1);
            playerpos.y = Utils::clamp(playerpos.y, 1, tiles.at(playerpos.x).size() - 1);

            otherplayerpos.x = Utils::clamp(otherplayerpos.x, 1, tiles.size() - 1);
            otherplayerpos.y = Utils::clamp(otherplayerpos.y, 1, tiles.at(otherplayerpos.x).size() - 1);

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
            if (tiles.at(playerpos.x).at(playerpos.y) == cave_tile::block) {
                collisionI->setLocation((playerpos + glm::vec2(0, 0)) * glm::vec2(16));
            }

            player_character->Update(delta.count(), std::vector<PhysX::collisionBox*>({ collisionA,collisionB,collisionC,collisionD,collisionE,collisionF,collisionG,collisionH ,collisionI }));
            player_gun->SetLocation(player_character->GetLocation());

            
            if (tiles.at(otherplayerpos.x + 1).at(otherplayerpos.y) == cave_tile::block)
                collisionA->setLocation((otherplayerpos + glm::vec2(1, 0)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x - 1).at(otherplayerpos.y) == cave_tile::block)
                collisionB->setLocation((otherplayerpos + glm::vec2(-1, 0)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x).at(otherplayerpos.y + 1) == cave_tile::block)
                collisionC->setLocation((otherplayerpos + glm::vec2(0, 1)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x).at(otherplayerpos.y - 1) == cave_tile::block)
                collisionD->setLocation((otherplayerpos + glm::vec2(0, -1)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x + 1).at(otherplayerpos.y + 1) == cave_tile::block)
                collisionE->setLocation((otherplayerpos + glm::vec2(1, 1)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x - 1).at(otherplayerpos.y + 1) == cave_tile::block)
                collisionF->setLocation((otherplayerpos + glm::vec2(-1, 1)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x + 1).at(otherplayerpos.y - 1) == cave_tile::block)
                collisionG->setLocation((otherplayerpos + glm::vec2(1, -1)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x - 1).at(otherplayerpos.y - 1) == cave_tile::block)
                collisionH->setLocation((otherplayerpos + glm::vec2(-1, -1)) * glm::vec2(16));
            if (tiles.at(otherplayerpos.x).at(otherplayerpos.y) == cave_tile::block)
                collisionI->setLocation((otherplayerpos + glm::vec2(0, 0)) * glm::vec2(16));

            other_character->Update(delta.count(), std::vector<PhysX::collisionBox*>({ collisionA,collisionB,collisionC,collisionD,collisionE,collisionF,collisionG,collisionH ,collisionI }));
            other_gun->SetLocation(other_character->GetLocation());

            if (GetTime() - last_update >= location_update_interval) {
                GetSteamManager()->SendMessageP2P_Reliable(0, "1", SteamTools::DataType::GAME_STATE, std::vector<std::string>({
                std::to_string(GetCharacter()->GetLocation().x),std::to_string(GetCharacter()->GetLocation().y) })); // SEND {1,LOCX,LOCY} PLAYER POSITION

                GetSteamManager()->SendMessageP2P_Reliable(0, "3", SteamTools::DataType::GAME_STATE, std::vector<std::string>({
                std::to_string(GetCharacter()->GetVelocity().x),std::to_string(GetCharacter()->GetVelocity().y) })); // SEND {1,LOCX,LOCY} PLAYER VELOCITY

                GetSteamManager()->SendMessageP2P_Reliable(0, "4", SteamTools::DataType::GAME_STATE, std::vector<std::string>({ std::to_string(player_character->GetHealth()) }));

                GetSteamManager()->SendMessageP2P_Reliable(0, "6", SteamTools::DataType::GAME_STATE, std::vector<std::string>({ std::to_string(GetCursorWorldPosition().x),std::to_string(GetCursorWorldPosition().y) })); // SEND CURSOR LOCATION

                last_update = GetTime();
            }


            if (player_character->GetHealth() <= 0 && GetTime() > (last_point + 3)) {
                other_points++;
                GetSteamManager()->SendMessageP2P_Reliable(0, "5", SteamTools::DataType::GAME_STATE, std::vector<std::string>({ std::to_string(my_points),std::to_string(other_points) }));
                
                if(other_points < 3 && my_points < 3)
                    StartRound();

                last_point = GetTime();
            }

            player_gun->UpdateCooldown(GetTime());
            other_gun->UpdateCooldown(GetTime());

            if (GetSteamManager()->GetNetworkStatus() == SteamTools::NetworkStatus::connected) {
                if (GetTime() - last_msg > 5) {
                    GetSteamManager()->Disconnect();
                    this->my_points = 3;
                    this->other_points = 0;
                }
            }

            this->player_ability->Update(GetTime());
            this->other_ability->Update(GetTime());

            this->player_character->SetCursorPosition(GetCursorWorldPosition());
        }
    }
}

void Application::Shooter::Start(int seed)
{
    int my_seed = cave_generator->Generate(seed);

    player_character->GetKinematics()->SetGravity(glm::vec2(0, -9.8));
    other_character->GetKinematics()->SetGravity(glm::vec2(0, -9.8));

    if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::host) {
        GetSteamManager()->SendMessageP2P_Reliable(0,"0", SteamTools::DataType::GAME_STATE, std::vector<std::string>({ std::to_string(my_seed) })); // SEND {0,SEED} WORLD GENERATION
        player_character->SetLocation(glm::vec2(64 * 16, 32 * 16 - 1));

        player_ability = new Abilities::Teleport(player_character);
        other_ability = new Abilities::GravityFlip(other_character);
    } else {
        player_character->SetLocation(glm::vec2((256-64) * 16, 32 * 16 - 1));

        player_ability = new Abilities::GravityFlip(player_character);
        other_ability = new Abilities::Teleport(other_character);
    }

    other_ability->SetCave(cave_generator);
    player_ability->SetCave(cave_generator);
    player_character->GetKinematics()->SetVelocity(glm::vec2(0));
    GetSteamManager()->SendMessageP2P_Reliable(0, "2", SteamTools::DataType::GAME_STATE, std::vector<std::string>({ std::to_string(player_character->GetHue()) })); // SEND {0,SEED} WORLD GENERATION
    started = true;
}

void Application::Shooter::StartRound() {
    if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::host) {
        player_character->SetLocation(glm::vec2(64 * 16, 32 * 16));
    } else {
        player_character->SetLocation(glm::vec2((256 - 64) * 16, 32 * 16));
    }
    player_character->SetHealth(100);
    other_character->SetHealth(100);
}