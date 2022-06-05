#ifndef APP
#define APP


#include "../../src/engine/engine.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"

namespace Application{
	class Boxer : public Game::Engine {
        public:
            void Begin();
            void GameLoop();
            void UILoop();
            void InputLoop();

        private:
            void DrawBackground();
            void DrawPlayer();

            bool ShouldSpawnEnemies();
            int  EnemySpawnAmount();
            void SpawnEnemies(int amount);

            void ProcessEnemies();
            void DrawEnemies();

            void DrawProtect();

            void Damage();


            Application::Player* player;
            Bloom::Text* timer;

            Bloom::Sprite* protect;

            int move_radius = 30;

            Bloom::Framebuffer* move_circle_buffer;
            int circle_data_uid;
            int circle_time_uid;
            int circle_position_uid;
            int protect_color_uid;

            bool spawned_wave = false;
            std::vector<Application::Enemy*> enemies;

            int health = 3;
            glm::vec2 camera_location = glm::vec2(0, 0);
	};
}

#endif // !BOXER