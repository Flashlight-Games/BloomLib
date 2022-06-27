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

            Bloom::Sprite* sprite_block;
            int block_color_uid = 0;
            float block_color[3] = {1,1,1};

            Bloom::Chromatic_Aberration* framebuffer_ca;
            float ca_size = 0.000;

            Bloom::Bloom_Framebuffer* framebuffer_bloom;
            int bloom_size = 0;
	};
}
#endif