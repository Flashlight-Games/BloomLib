#ifndef BLOOM_APP
#define BLOOM_APP

#include "Bloom/engine/engine.h"

namespace Application {
    class App : public Game::Engine {
        using Game::Engine::Engine;
    public:
        void Begin();
        void GameLoop();
        void UILoop();
        void InputLoop();
        void Update();

        ~App();
        void step();
    private:
        // HERE GO VARIABLES
        Bloom::Sprite* test;
        Bloom::WFC::WaveFunctionCollapse* wfc;
        void showgrid();
    };
}
#endif