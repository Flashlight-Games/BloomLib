#pragma once
#include "../../src/engine/engine.h"

namespace Application{
	class Testing : public Game::Engine {
        public:
            void Begin();
            void GameLoop();
            void UILoop();
            void InputLoop();
            
        private:
            Bloom::Sprite* endBox;

            float ABoxPos[2] = { 0,0 };
            float BBoxPos[2] = { 0,0 };
            float Rots[2] = { 0,0 };
	};
}