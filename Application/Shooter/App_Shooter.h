#ifndef APP
#define APP

#include "engine/engine.h"
#include "Cave/Cave.h"
#include "Player/Player.h"
#include "Guns/Guns.h"
#include "Abilities/Ability.h"
#include "Abilities/TestAbility.h"

namespace Application{
	class Shooter : public Game::Engine {
        using Game::Engine::Engine;
        public:
            void Begin();
            void GameLoop();
            void UILoop();
            void InputLoop();

            void SetStatusString(SteamTools::NetworkStatus status, SteamTools::NetworkPosition position);
            void ParseNetworkMessage(SteamTools::Message message);

            void SetGamepad(int gamepad) { jid = gamepad; };

            Cave* GetCave() { return cave_generator; };
            auto GetCharacter() { return player_character; };

            void Update();
            void Start(int seed = -1);
            auto GetStarted() { return started; };
            auto GetInput() { return character_input; };

            glm::vec2 GetCursorWorldPosition() { return (GetCursorPosition() - (GetPixelWindowSize() / glm::vec2(2)) + GetCameraWorldPosition()); }
            glm::vec2 GetCameraWorldPosition() { return (GetPixelWindowSize() / glm::vec2(2)) - camera_location; };

            void PlayerFire() { player_gun->Fire(GetTime(), cave_generator, other_character); };
            void PlayerZoom() { zoom = true; };
            void PlayerStopZoom() { zoom = false; }
            auto GetPlayerAimAngle() { return player_gun->GetAngle(); };
            auto GetPlayerAmmo() { return player_gun->GetAmmo(); };

            void StartRound();
            void ResetPoints() { my_points = 0; other_points = 0; };

            void ActivateAbility() { this->player_ability->Activate(GetTime()); };
            void DeactivateAbility() { this->player_ability->Deactivate(GetTime()); };
        private:

            glm::vec2 camera_location = glm::vec2(0);
            Bloom::Text* text_pressstart;

            Player* player_character;
            glm::vec2 character_input;
            Gun* player_gun = new TEST_GUN;
            Ability* player_ability;

            Player* other_character;            
            glm::vec2 other_input;
            Gun* other_gun = new TEST_GUN;
            Ability* other_ability = new Abilities::Test(other_character);

            int jid = -1;

            Cave* cave_generator;

            // NETWORK
            std::string status_string = "OFFLINE";
            CSteamID current_lobby;

            PhysX::collisionBox* collisionA;
            PhysX::collisionBox* collisionB;
            PhysX::collisionBox* collisionC;
            PhysX::collisionBox* collisionD;
            PhysX::collisionBox* collisionE;
            PhysX::collisionBox* collisionF;
            PhysX::collisionBox* collisionG;
            PhysX::collisionBox* collisionH;
            PhysX::collisionBox* collisionI;
            std::vector<PhysX::collisionBox*> boxes;

            bool started = false;
            float last_update = 0.0;
            float location_update_interval = 0.1;

            glm::vec2 zoom_location;
            bool zoom = false;

            int my_points = 0;
            int other_points = 0;

            float last_point = 0;
            float last_msg = 0.0;
	};
}
#endif