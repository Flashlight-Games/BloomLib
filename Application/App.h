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
        void SetZoomTarget(float zoom);
    private:
        // HERE GO VARIABLES
        glm::vec2 cam_loc;
        Bloom::Sprite* test;
        float zoom_target = 1.0;
    };
}
#endif