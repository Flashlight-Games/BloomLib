#include "App_Boxer.h"

int main()
{
    Game::configuration settings;
    settings.resolution_window_size = glm::vec2(960, 960);
    settings.resolution_pixels_size = glm::vec2(192, 192);
    settings.rendering_enable_vsync = false;

    Application::Boxer App = Application::Boxer(settings);
    return App.Run(); 
}