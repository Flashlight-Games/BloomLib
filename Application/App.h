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
        void print();
    private:
        // HERE GO VARIABLES
        Bloom::Sprite* test;
        Bloom::WFC::WaveFunctionCollapse* wfc;
        void showgrid();
        void showbitmap(std::vector<std::vector<int>> bitmap);
        std::vector<std::vector<int>> bitmap;
    };
}
#endif