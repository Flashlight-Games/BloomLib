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

            auto GetTargetIP() { return this->target_ip; };

            void SetHost(ENetHost* host) { this->host = host; };
            auto GetHost() { return this->host; };
            
            void SetIsServer(bool isServer) { this->isServer = isServer; };
            void SetOther(ENetPeer* other) { this->other = other; };
            auto GetOther() { return this->other; };
        private:
            Bloom::Text* text_pressstart;

            Bloom::Sprite* sprite_block;
            int block_color_uid = 0;
            float block_color[3] = {5,5,5};

            PhysX::collisionBox* mainBox;

            Application::Checker* checker;

            ENetHost* host;
            bool isServer = false;
            ENetPeer* other;
            char target_ip[15] = {"127.0.0.1"};
	};
}
#endif