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
    private:
        // HERE GO VARIABLES
        Bloom::SubdividedSprite* testSprite;
        Bloom::Sprite* referenceSprite;
    };
}
#endif