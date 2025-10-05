#ifndef BLOOM_APP
#define BLOOM_APP

#include "Bloom/dependencies.h"

import Bloom.Engine;
import Bloom.ECS;
import Bloom.Utils;

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
    private:
        bool shouldClose = false;
        ECS::World world;
        int UPS = 0; // world updates per second

        Bloom::Sprite* test;
    };
}
#endif