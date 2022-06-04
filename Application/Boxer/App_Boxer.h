#ifndef APP
#define APP


#include "../../src/engine/engine.h"
#include "Player/Player.h"

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

            Application::Player* player;
            Bloom::Text* timer;

            int move_radius = 30;

            Bloom::Framebuffer* move_circle_buffer;
            int circle_data_uid;
            int circle_time_uid;
            int circle_position_uid;

            bool spawned_wave = false;
	};
}

#endif // !BOXER