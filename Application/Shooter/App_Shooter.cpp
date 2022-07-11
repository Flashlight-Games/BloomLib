#include "App_Shooter.h"

using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = Utils::GetApp<Application::Shooter>(window);
    switch (key) {
    case GLFW_KEY_H:
        if (action == GLFW_PRESS) {
            if (app->GetHost() == NULL) {
                app->SetHost(Network::Server_Create(7777, 10));
                app->SetIsServer(true);
            }
            else
                std::cout << "HOST ALREADY CREATED!\n";
        }
        break;
    case GLFW_KEY_J:
        if (action == GLFW_PRESS) {
            if (app->GetHost() == NULL) {
                app->SetHost(Network::Client_Create());
                app->SetIsServer(false);
                auto other = Network::Client_ConnectToServer(app->GetHost(), app->GetTargetIP(), 7777);
                if (other == NULL)
                    app->SetHost(NULL);
                else {
                    app->SetOther(other);
                    std::cout << "SUCCESS!\n";
                }
            }
            else
                std::cout << "HOST ALREADY CREATED!\n";
        }
        break;
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            if(app->GetHost() != NULL)
                enet_host_destroy(app->GetHost());
            glfwSetWindowShouldClose(app->GetWindow(), true);
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

    text_pressstart = new Bloom::Text("res/engine/fonts/PressStart/PressStart.ttf",GetViewMatrix(),GetWindowProjectionMatrix(),-0.5, 512);

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
    ImGui::InputText("IP", target_ip, sizeof(target_ip));
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
    if (host != NULL) {
        ENetEvent event;
        if (isServer) {
            event = Network::Server_ReceiveData(host);
            if (Network::isConnectionEvent(event))
                other = event.peer;
        } else {
            event = Network::Client_ReceiveData(host);
        }
        if (Network::isEvent(event))
            if (Network::isReceiveEvent(event))
                GetSptiteBlock()->SetLocation(glm::vec2(GetSptiteBlock()->GetLocation().x, std::stof(Network::GetEventData(event))));
    }

    //RENDERING
    EnablePixels();

    sprite_block->GetShader()->use();
    sprite_block->GetShader()->uniformVec4(block_color_uid, glm::vec4(block_color[0], block_color[1], block_color[2], 1));
    sprite_block->SetRotation((GetTime() / 10.0) * 360);
    sprite_block->Draw(GetViewMatrix());

    mainBox->setLocation(sprite_block->GetLocation());
    mainBox->setRotation(sprite_block->GetRotation());
    mainBox->setScale(sprite_block->GetSize());

    checker->Draw(GetTime(), GetGameSpeed());
    
    std::string status = "OFFLINE";
    if (GetHost() != NULL) {
        if (isServer)
            status = "HOST";
        else
            status = "CONNECTED";
    }
    else
        std::string status = "OFFLINE";

    text_pressstart->Print(GetWindowSize() / glm::vec2(2) + glm::vec2(0, -150), status.c_str(), 1, GetWindowSize(), glm::vec4(1), true);
    DrawPixelsToDefault();
}

void Shooter::InputLoop() {
    if (Utils::isKeyPressed(GetWindow(), GLFW_KEY_SPACE)) {
        auto newLocation = GetPixelWindowSize() / glm::vec2(2) + glm::vec2(0, 50 * Utils::Noise_Simplex(glm::vec2(0, GetTime())) / 2);
        GetSptiteBlock()->SetLocation(newLocation);
        if (GetOther() != NULL) {
            Network::SendPacket(GetOther(), std::to_string(newLocation.y).c_str());
        }
    }
}