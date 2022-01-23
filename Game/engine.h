#ifndef GAME
#define GAME

#include "../src/renderer/renderer.h"

namespace Game{
    void GameLoop();
    void InputLoop();
    void UILoop();

    class Engine{
        public:
        Engine();
        void Update();

        Bloom::Renderer GetRenderer();

        private:
        Bloom::Renderer renderer = Bloom::Renderer(
            glm::vec2(800,600), // WINDOW SIZE
            glm::vec2(64,48),   // PIXEL SIZE
            nullptr,          // GAME FUNC
            nullptr,         // INPUT FUNC
            UILoop,            // UI FUNC
            "Bloom",            // WINDOW NAME
            false,              // RESIZEABLE
            false,               // BORDERLESS
            false,              // MAXIMISED
            true                // TRANSPARENT
            );
    };
}
#endif