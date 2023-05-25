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
    case GLFW_KEY_M:
        if (action == GLFW_PRESS) {
            app->step();
        }
        break;
    case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS) {
            app->print();
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
    showbitmap(this->bitmap);
    showgrid();
    DrawPixelsToDefault();
}

void Application::App::InputLoop() {
}

void Application::App::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);

    const char* input = "include/Bloom/res/engine/textures/wfc_water.png";

    this->test = new Bloom::Sprite(input, 1, GetViewMatrix(), GetPixelProjectionMatrix());
    this->test->SetShader(new Bloom::Shader("include/Bloom/res/engine/shaders/sprite/spriteshader_vertex.glsl", "include/Bloom/res/engine/shaders/sprite/spriteshader_wfc_fragment.glsl"),GetPixelProjectionMatrix());
    this->test->SetLocation(this->test->GetSize());
    
    this->bitmap = Bloom::WFC::CreateBitmapFromImage(input);
    this->wfc = new Bloom::WFC::WaveFunctionCollapse(glm::vec2(16), Bloom::WFC::findPatterns(bitmap, glm::vec2(3)),Bloom::WFC::CalculateWeights(bitmap));

    update_thread = new std::thread(&Application::App::Update, this);
    //glClearColor(0.21, 0.21, 0.21, 1.0);
}

void Application::App::Update() {
    this->step();
    while (!glfwWindowShouldClose(GetWindow())) {

    }

    std::terminate();
    delete update_thread;
}

Application::App::~App()
{
    delete this->test;
    delete this->wfc;
}

void Application::App::step()
{
    
    if (!failed) {
        std::cout << "STEP!\n";
        wfc->Print();
        if (!wfc->Collapse()) {
            //failed = true;
            //wfc->InitGrid();
        }
    }
    /*if (wfc->isGridDone())
        wfc->Print();*/
}

void Application::App::print()
{
    this->wfc->Print();
}

void Application::App::showgrid()
{
    auto grid = *this->wfc->getGrid();
    if (true) {
        for (int y = 0; y < grid.size(); y++)
        {
            for (int x = 0; x < grid.size(); x++)
            {
                if(grid.at(y).at(x).GetEntropy()>1)
                    DrawDebugPoint((glm::vec2(x, grid.size()-y))*1.0f + glm::vec2(10), 1, glm::vec3(grid.at(y).at(x).GetEntropy() / (float)wfc->getMaxVal()));
                else {
                    DrawDebugPoint((glm::vec2(x, grid.size()-y)) * 1.0f + glm::vec2(10), 1, Utils::HSVtoRGB(glm::vec3((grid.at(y).at(x).superposition.at(0) / (float)wfc->getMaxVal()) * 180.0f, 100, 100)));
                }
            }
        }
    }
}

void Application::App::showbitmap(std::vector<std::vector<int>> bitmap)
{
    for (int y = 0; y < bitmap.size(); y++)
    {
        for (int x = 0; x < bitmap.at(y).size(); x++)
        {
            DrawDebugPoint((glm::vec2(x, bitmap.size() - y)) * 1.0f + glm::vec2(30,10), 1, Utils::HSVtoRGB(glm::vec3(bitmap.at(y).at(x) / (float)wfc->getMaxVal() * 180.0f, 100, 100)));
        }
    }
}

