#include "App.h"

//using namespace Application;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = Utils::GetApp<Application::App>(window);
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            app->ExitGame();
        }
        break;
    }
}

void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    auto app = Utils::GetApp<Application::App>(window);
    switch (key) {
    case GLFW_MOUSE_BUTTON_LEFT:
        break;
    }
}

void Application::App::UILoop() {
    ImGui::Begin("Control window"); {
        if (Utils::isDevelopmentConfiguration()) {
            ImGui::TextColored(ImVec4(1, 0.5, 0.0, 1), (std::string("GLFW v_").c_str() + std::string(glfwGetVersionString())).c_str());
        }
    } ImGui::End();

    if (Utils::isDevelopmentConfiguration()) {
        ImGui::Begin("Performance window");
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
        ImGui::End();
    }
}

void Application::App::GameLoop() {
    // RENDERING
    EnablePixels();
    this->test->Draw();
    showgrid();
    DrawPixelsToDefault();
}

void Application::App::InputLoop() {
}

void Application::App::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);
    this->test = new Bloom::Sprite("include/Bloom/res/engine/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix());
    this->test->SetLocation(glm::vec2(20));

    std::vector<std::vector<int>> bitmap;

    update_thread = new std::thread(&Application::App::Update, this);
    //glClearColor(0.21, 0.21, 0.21, 1.0);
}

void Application::App::Update() {
    this->wfc = new Bloom::WFC::WaveFunctionCollapse(glm::vec2(5), Bloom::WFC::findPatterns(Bloom::WFC::CreateBitmapFromImage("include/Bloom/res/engine/textures/wfc_house.png"), glm::vec2(3)));
    while (!glfwWindowShouldClose(GetWindow())) {

    }

    std::terminate();
    delete update_thread;
}

Application::App::~App()
{
    delete this->test;
}

void Application::App::showgrid()
{
    if (GetTime() > 100) {
        for (int y = 0; y < this->wfc->getGrid().size(); y++)
        {
            for (int x = 0; x < this->wfc->getGrid().at(0).size(); x++)
            {
                DrawDebugPoint(glm::vec2(x, y), 1, glm::vec3(this->wfc->getGrid().at(y).at(x).GetEntropy() / 5.0) * 255.0f);
            }
        }
    }
}
