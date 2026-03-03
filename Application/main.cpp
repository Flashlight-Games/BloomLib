#include "App.h"

int main()
{
    Game::configuration settings;
    settings.resolution_window_size = glm::vec2(1280, 720);
    settings.resolution_pixels_size = Utils::PixelateResolution(settings.resolution_window_size, 16 * 2);
    settings.rendering_enable_vsync = false;
    settings.window_name = "Test Game";
    settings.glfw_transparent = false;
    settings.feature_enable_steam = false;

    Application::App App = Application::App(settings);
    App.Run();
}