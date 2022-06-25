#ifndef APP
#define APP

#include "../../src/engine/engine.h"

namespace Application{
	class Shooter : public Game::Engine {
        using Game::Engine::Engine;
        public:
            void Begin();
            void GameLoop();
            void UILoop();
            void InputLoop();

        private:
            Bloom::Text* text_pressstart;
	};
}
#endif