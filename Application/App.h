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
        void Begin() override;
        void InputLoop() override;
        void GameLoop() override;
        void UILoop() override;
        void Update(float deltaTime) override;

        ~App();
    private:
        ECS::World world;

        Bloom::Sprite* test;
    };
}
#endif