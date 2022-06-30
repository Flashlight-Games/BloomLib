#include "App_Shooter.h"

int main()
{
    Game::configuration settings;
    settings.resolution_window_size = glm::vec2(960, 960);
    settings.resolution_pixels_size = glm::vec2(192, 192);
    settings.rendering_enable_vsync = true; // For game to boxer correctly, disable vsync
    settings.window_name = "Test Game";
    settings.glfw_transparent = false;

    Application::Shooter App = Application::Shooter(settings);
    return App.Run(); 
}