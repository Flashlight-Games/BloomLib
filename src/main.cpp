#include "modules.h"

#include <iostream>
#include <math.h>
#include <string>
#include <chrono>

#include "../Game/engine.h"

int main()
{ 
    Game::Engine reality = Game::Engine();

    int boxPosX = 0;
    int boxPosY = 0;
    float time = 0;

    Bloom::Sprite spritetest = Bloom::Sprite("res/textures/detailTest16Line.png",1,reality.GetRenderer().getViewMatrix(),reality.GetRenderer().getPixelProjectionMatrix());
    Bloom::Spritesheet sheettest = Bloom::Spritesheet("res/textures/2x8_SpritesheetTest.png",1,glm::vec2(2,8),reality.GetRenderer().getViewMatrix(),reality.GetRenderer().getPixelProjectionMatrix());

    glClearColor(0.21,0.21,0.21,0.5);

    const char* fontPath = "res/fonts/Roboto-Thin.ttf";
    Bloom::Text fontTesting = Bloom::Text(fontPath,128);

    auto beginTime = std::chrono::system_clock::now();
    auto currentTime = beginTime;
    std::chrono::duration<double> render_duration;
    std::chrono::duration<double> transform_duration;
    std::chrono::duration<double> font_duration;
    std::chrono::duration<double> imGui_duration;
    std::chrono::duration<double> frameBuffer_duration;
    std::chrono::duration<double> bufferSwap_duration;

    beginTime = std::chrono::system_clock::now(); // Check begin time
    currentTime = std::chrono::system_clock::now();

    bufferSwap_duration = std::chrono::system_clock::now()-currentTime;
    currentTime = std::chrono::system_clock::now();

    transform_duration = std::chrono::system_clock::now()-currentTime;
    currentTime = std::chrono::system_clock::now();
    
    frameBuffer_duration = std::chrono::system_clock::now()-currentTime;
    currentTime = std::chrono::system_clock::now();

    imGui_duration = std::chrono::system_clock::now()-currentTime;
    currentTime = std::chrono::system_clock::now();

    render_duration = std::chrono::system_clock::now()-beginTime;
    currentTime = std::chrono::system_clock::now();

    while(!glfwWindowShouldClose(reality.GetRenderer().GetWindow())) // GAME LOOP
    {
        reality.Update();

        // SPRITE
        // spritetest.SetLocation(glm::vec2(boxPosX,boxPosY));
        // spritetest.SetRotation(sin(time)*180);
        // spritetest.Draw();

        // ANIMATED SPRITE
        // sheettest.Draw();
        // sheettest.TimedAdvance(0.1);

        // TEXT RENDERING
        // fontTesting.Print(glm::vec2(0*windowWidth/pixel_windowWidth,boxPosY*windowHeight/pixel_windowHeight),"I don't\nknow if this text's\nspacing is correct...",glm::vec2(windowWidth,windowHeight),glm::vec4(0,1,0,1));
        // font_duration = std::chrono::system_clock::now()-currentTime;
        // currentTime = std::chrono::system_clock::now();
    }

    glfwTerminate();
    return 0;
}