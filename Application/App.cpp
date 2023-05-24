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
    case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS) {
            app->step();
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
    //this->test->Draw();
    this->step();
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

    this->wfc = new Bloom::WFC::WaveFunctionCollapse(glm::vec2(10), Bloom::WFC::findPatterns(Bloom::WFC::CreateBitmapFromImage("include/Bloom/res/engine/textures/wfc_test.png"), glm::vec2(3)));

    update_thread = new std::thread(&Application::App::Update, this);
    //glClearColor(0.21, 0.21, 0.21, 1.0);
}

void Application::App::Update() {
    while (!glfwWindowShouldClose(GetWindow())) {

    }

    std::terminate();
    delete update_thread;
}

Application::App::~App()
{
    delete this->test;
}

void Application::App::step()
{
    if (!wfc->Collapse())
        wfc->InitGrid();
    if (wfc->isGridDone())
        wfc->Print();
}

void Application::App::showgrid()
{
    auto grid = *this->wfc->getGrid();
    if (true) {
        for (int y = 0; y < grid.size(); y++)
        {
            for (int x = 0; x < grid.size(); x++)
            {
                DrawDebugPoint((glm::vec2(x, y))*1.0f + glm::vec2(10), 1, glm::vec3(grid.at(y).at(x).GetEntropy() / (float)wfc->getMaxVal(), 0.0, 0.0));
            }
        }
    }
}
