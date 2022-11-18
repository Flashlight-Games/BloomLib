#include "App.h"

int main()
{
    Game::configuration settings;
    settings.resolution_window_size = glm::vec2(1280, 720);
    settings.resolution_pixels_size = Utils::PixelateResolution(settings.resolution_window_size, 16);
    settings.rendering_enable_vsync = false; // For game to boxer correctly, disable vsync
    settings.window_name = "Test Game";
    settings.glfw_transparent = false;

    Application::App App = Application::App(settings);
    return App.Run();
}