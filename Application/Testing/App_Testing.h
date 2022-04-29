#pragma once
#include "../../src/engine/engine.h"

//using namespace Game;

namespace Application{
	class Testing : public Game::Engine {
        public:
            void Begin();
            void GameLoop();
            void UILoop();
        
        private:
            Bloom::Sprite* endBox;

            int ABoxPos[2] = { 0,0 };
            int BBoxPos[2] = { 0,0 };
            int Rots[2] = { 0,0 };
	};
}