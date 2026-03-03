#include "App.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = static_cast<Application::App*>(glfwGetWindowUserPointer(window));
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            app->ExitGame();
        }
        break;
    }
}

static void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    auto app = static_cast<Application::App*>(glfwGetWindowUserPointer(window));
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto app = static_cast<Application::App*>(glfwGetWindowUserPointer(window));
}


void Application::App::UILoop() {
    #ifdef DEBUG
        ImVec2 last_size;
        ImVec2 last_pos;
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(400, 75));
        ImGui::Begin("Performance window", nullptr, ImGuiWindowFlags_NoCollapse); {
            ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Time is %s", std::to_string((int)GetTime()).c_str());
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS", std::to_string(GetFPS()).c_str());
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s FPS Average", std::to_string(GetAverageFPS()).c_str());
            last_size = ImGui::GetWindowSize(); last_pos = ImGui::GetWindowPos();
        }ImGui::End();


        ImGui::SetNextWindowPos(ImVec2(10, last_pos.y+10+last_size.y));
        ImGui::SetNextWindowSize(ImVec2(400, 75));
        ImGui::Begin("Control window", nullptr, ImGuiWindowFlags_NoCollapse); {
            ImGui::TextColored(ImVec4(1, 0.5, 0.0, 1), (std::string("GLFW v_").c_str() + std::string(glfwGetVersionString())).c_str());
            last_size = ImGui::GetWindowSize(); last_pos = ImGui::GetWindowPos();
        } 
        ImGui::End();
    #endif
}

void Application::App::GameLoop() {
    // RENDER PIXEL BACKGROUND
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    this->test->SetRotation(std::sin(GetTime() / 3.0f) * 180.f);
    this->test->Draw();
}

void Application::App::InputLoop() {
    
}

void Application::App::Begin() {
    glfwSetKeyCallback(GetWindow(), key_callback);
    glfwSetMouseButtonCallback(GetWindow(), mouse_callback);
    glfwSetScrollCallback(GetWindow(), scroll_callback);
    this->test = new Bloom::Sprite("Application/res/textures/rect16.png", 1, GetViewMatrix(), GetPixelProjectionMatrix());
    this->test->SetLocation(glm::vec2(12));
}

void Application::App::Update(float deltaTime) {
    world.update(deltaTime);
}

Application::App::~App() {
    #ifdef DEBUG
        std::cout << Utils::Green() << "APP CLOSED SUCCESSFULY\n" << Utils::Default();
    #endif
}