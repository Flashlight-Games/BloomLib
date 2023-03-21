#include "App.h"

int main()
{
    Game::configuration settings;
    settings.resolution_window_size = glm::vec2(1280, 720);
    settings.resolution_pixels_size = glm::vec2(Utils::PixelateResolution(settings.resolution_window_size,16));
    settings.rendering_enable_vsync = false;
    settings.window_name = "Test Game";
    settings.glfw_transparent = false;
    settings.feature_enable_steam = true;

    Application::App App = Application::App(settings);
    return App.Run();
}