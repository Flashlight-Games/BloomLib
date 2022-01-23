#include "engine.h"

using namespace Game;

Engine::Engine(){};

void Game::GameLoop(){

}

void Game::InputLoop(){
    // if(glfwGetKey(engine.GetRenderer().GetWindow(),GLFW_KEY_ESCAPE))
        // glfwSetWindowShouldClose(engine.GetRenderer().GetWindow,true);
}

void Game::UILoop()
{
        ImGui::Begin("Control window");
            ImGui::TextColored(ImVec4(1,0.5,0,1),"Time is %s",std::to_string(((int)0)).c_str());
            // ImGui::SliderInt("Box PosX",&boxPosX,0,bloom.GetPixelSize()[0]);
            // ImGui::SliderInt("Box PosY",&boxPosY,0,bloom.GetPixelSize()[1]);
        ImGui::End();
    
        // ImGui::Begin("Performance window");
        //     ImGui::ProgressBar(transform_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Transform");
        //     ImGui::ProgressBar(frameBuffer_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Framebuffer");
        //     ImGui::ProgressBar(imGui_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"ImGui");
        //     ImGui::ProgressBar(font_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Font");

        //     ImGui::ProgressBar(bufferSwap_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"BufferSwap");
        //     ImGui::TextColored(ImVec4(1,1,0,1),"%s FPS",std::to_string(bloom.GetFPS()).c_str());
        // ImGui::End();
}

Bloom::Renderer Engine::GetRenderer(){
    return this->renderer;
}

void Engine::Update(){
    this->renderer.Update();
}