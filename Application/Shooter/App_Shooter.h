#ifndef APP
#define APP

#include "engine/engine.h"
#include "Checker/Checker.h"

namespace Application{
	class Shooter : public Game::Engine {
        using Game::Engine::Engine;
        public:
            void Begin();
            void GameLoop();
            void UILoop();
            void InputLoop();

            auto GetChecker() { return this->checker; };
            auto GetmainBox() { return this->mainBox; };
            auto GetSptiteBlock() { return this->sprite_block; };

            void SetStatusString(SteamTools::NetworkStatus status);
        private:
            Bloom::Text* text_pressstart;

            Bloom::Sprite* sprite_block;
            int block_color_uid = 0;
            float block_color[3] = {5,5,5};

            PhysX::collisionBox* mainBox;

            Application::Checker* checker;

            // NETWORK
            std::string status_string = "OFFLINE";
            CSteamID current_lobby;
	};
}
#endif