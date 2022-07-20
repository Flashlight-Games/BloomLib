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

            void SetStatusString(SteamTools::NetworkStatus status, SteamTools::NetworkPosition position);
            void ParseNetworkMessage(SteamTools::Message message);

            void PhysicsBoost() { test_force += glm::vec2(500,0); };
        private:
            Bloom::Text* text_pressstart;

            Bloom::Sprite* sprite_block;
            PhysX::collisionBox* mainBox;
            
            float main_anlge = 0;
            int block_color_uid = 0;
            float block_color[3] = {5,5,5};


            Bloom::Sprite* test_block;
            PhysX::collisionBox* testBox;
            float pixels_to_meters = 25;
            float test_mass = 5; // KG
            float test_inforce = 50;
            glm::vec2 test_location;
            glm::vec2 test_force;
            glm::vec2 test_acceleration;
            glm::vec2 test_velocity;

            Application::Checker* checker;

            // NETWORK
            std::string status_string = "OFFLINE";
            CSteamID current_lobby;
	};
}
#endif