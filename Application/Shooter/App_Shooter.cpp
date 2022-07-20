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
    case GLFW_KEY_H:
        if(action == GLFW_PRESS)
            if (app->GetSteamManager()->GetNetworkStatus() == SteamTools::NetworkStatus::connected && app->GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::lobby)
                app->GetSteamManager()->HostP2P();
        break;
    case GLFW_KEY_J:
        if (action == GLFW_PRESS)
            if (app->GetSteamManager()->GetNetworkStatus() == SteamTools::NetworkStatus::connected && app->GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::lobby)
                app->GetSteamManager()->ConnectP2P(SteamMatchmaking()->GetLobbyOwner(app->GetSteamManager()->GetCurrentLobby()));
        break;
    case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS) {
            auto newLocation = app->GetPixelWindowSize() / glm::vec2(2) + glm::vec2(0, 50 * Utils::Noise_Simplex(glm::vec2(0, app->GetTime())) / 2);
            app->GetSptiteBlock()->SetLocation(newLocation);
            app->GetSteamManager()->SendMessageP2P_Reliable(0, std::to_string(key).c_str(), SteamTools::DataType::PLAYER_INPUT_KEY_PRESS,std::vector<std::string>({std::to_string(newLocation.y)})); // SEND PRESSED SPACE AND NEW Y
        }
    case GLFW_KEY_V:
        if (action == GLFW_PRESS) {
            app->PhysicsBoost();
        }
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
    sprite_block->SetScale(glm::vec2(4,1));

    sprite_block->GetShader()->use();
    block_color_uid = sprite_block->GetShader()->getUniformId("uColor");

    test_block = new Bloom::Sprite("res/engine/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix(), glm::vec2(0.5));
    testBox = new PhysX::collisionBox("main",test_block);

    mainBox = new PhysX::collisionBox("main", sprite_block);

    checker = new Application::Checker(25,GetViewMatrix(),GetPixelProjectionMatrix());
}

void Shooter::UILoop() {
    ImGui::Begin("Control window"); {
        //ImGui::ColorPicker3("BLOCK COLOR", block_color);
        ImGui::TextColored(ImVec4(0, 1, 1, 1), (std::string("FORCE X: ").c_str() + std::to_string(test_force.x)).c_str());
        ImGui::TextColored(ImVec4(0, 1, 1, 1), (std::string("FORCE Y: ").c_str() + std::to_string(test_force.y)).c_str());
        ImGui::TextColored(ImVec4(0, 1, 0, 1), (std::string("ACCEL X: ").c_str() + std::to_string(test_acceleration.x)).c_str());
        ImGui::TextColored(ImVec4(0, 1, 0, 1), (std::string("ACCEL Y: ").c_str() + std::to_string(test_acceleration.y)).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), (std::string("VELOCITY X: ").c_str() + std::to_string(test_velocity.x)).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), (std::string("VELOCITY Y: ").c_str() + std::to_string(test_velocity.y)).c_str());
        ImGui::SliderFloat("FORCE", &test_inforce, -100, 100);
        ImGui::SliderFloat("MASS", &test_mass, 0, 10);
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
        GetSteamManager()->MatchmakeLoop_Quick(2); // UPDATE MATCHMAKING NETWORK STATUS
    } if (GetSteamManager()->GetNetworkPosition() == SteamTools::NetworkPosition::lobby) {
        // WAIT FOR READY
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

    sprite_block->GetShader()->use();
    sprite_block->GetShader()->uniformVec4(block_color_uid, glm::vec4(block_color[0], block_color[1], block_color[2], 1));
    sprite_block->SetRotation(45);
    sprite_block->Draw(GetViewMatrix());

    mainBox->setLocation(sprite_block->GetLocation());
    mainBox->setRotation(sprite_block->GetRotation());
    mainBox->setScale(sprite_block->GetSize());

    checker->Draw(GetTime(), GetGameSpeed());

    test_force -= test_velocity * glm::vec2(5) * test_mass; // FRICTION
    test_acceleration = test_force / glm::vec2(test_mass); // ACCELERATION
    
    test_velocity += test_acceleration * glm::vec2(GetDeltaTime() / 1000); // VELOCITY
    if (testBox->check_Traced(mainBox) == glm::vec2(INFINITY)) {
        //test_velocity += test_acceleration * glm::vec2(GetDeltaTime() / 1000);
    }
    else {
        if(glm::dot(glm::normalize(testBox->check_TracedNormal(mainBox)),glm::normalize(test_velocity)) < 0.0)
            test_velocity += (glm::ceil(glm::abs(test_velocity)) * testBox->check_TracedNormal(mainBox));
    }
    test_location += test_velocity * glm::vec2(GetDeltaTime() / 1000) * pixels_to_meters; // LOCATION

    test_block->SetLocation(test_location);
    testBox->setLocation(test_block->GetLocation());
    test_block->Draw(GetViewMatrix());

    DrawDebugLine(test_location, test_location + (testBox->check_TracedNormal(mainBox) * glm::vec2(20)), 1, glm::vec3(1, 0, 1));

    DrawPixelsToDefault(false);
}

void Shooter::InputLoop() {
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_SPACE)) {
        /*auto newLocation = GetPixelWindowSize() / glm::vec2(2) + glm::vec2(0, 50 * Utils::Noise_Simplex(glm::vec2(0, GetTime())) / 2);
        GetSptiteBlock()->SetLocation(newLocation);*/
    }
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_RIGHT))
        this->test_force.x = test_inforce;
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_LEFT))
        this->test_force.x = -test_inforce;
    if(!Utils::isKeyPressed(GetWindow(), GLFW_KEY_LEFT) && !Utils::isKeyPressed(GetWindow(), GLFW_KEY_RIGHT))
        this->test_force.x = 0.0;
        

    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_UP))
        this->test_force.y = test_inforce;
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_DOWN))
        this->test_force.y = -test_inforce;
    if (!Utils::isKeyPressed(GetWindow(), GLFW_KEY_UP) && !Utils::isKeyPressed(GetWindow(), GLFW_KEY_DOWN))
        this->test_force.y = 0.0;
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
            GetSptiteBlock()->SetLocation(glm::vec2(GetPixelWindowSize().x / 2, std::stof(message.flags.at(0))));
        }
        break;
    }
}
