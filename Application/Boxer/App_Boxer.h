#ifndef APP
#define APP

#include "../../src/engine/engine.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "Upgrade Health/UpgradeHealth.h"
#include "Upgrade Radius/UpgradeRadius.h"

namespace Application{
	class Boxer : public Game::Engine {
        using Game::Engine::Engine;
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
            void EraseEnemies();

            void DrawProtect();

            void Damage();
            void ProcessCamera();

            void DrawUpgrades();
            void ProcessUpgrades();

            void ProcessWaveEffect();

            void ProcessDeathMenu();
            void ProcessStartMenu();

            Application::Player* player;
            Bloom::Text* timer;

            Bloom::Sprite* protect;

            int start_radius = 30;
            int move_radius = start_radius;

            Bloom::Framebuffer* move_circle_buffer;
            int circle_data_uid;
            int circle_time_uid;
            int circle_position_uid;
            int protect_color_uid;

            Bloom::Framebuffer* wave_buffer;
            int wave_radius_uid;
            int wave_center_uid;

            Bloom::Framebuffer* transition_out_buffer;
            int transition_out_radius_uid;
            int transition_out_center_uid;
            int transition_out_color_uid;

            Bloom::Framebuffer* transition_in_buffer;
            int transition_in_radius_uid;
            int transition_in_center_uid;
            int transition_in_color_uid;

            bool spawned_wave = false;
            std::vector<Application::Enemy*> enemies;

            int start_health = 3;
            int health = start_health;
            int last_hp = health;
            glm::vec2 camera_location = glm::vec2(0, 0);
            Bloom::Shake* hitShakeX;
            Bloom::Shake* hitShakeY;

            Application::Upgrade_Health* upgrade_health;
            Application::Upgrade_Radius* upgrade_radius;

            std::thread* enemy_thread;
            std::thread* upgrade_thread;

            bool in_menu = true;
            float transition_out_start_time = 0;
            float transition_out_duration = 1.5;

            float transition_in_start_time = -1;
            float transition_in_duration = 1.5;

            bool start_menu = true;

            SoLoud::Wav* sound_hit;
            SoLoud::Wav* sound_lose;
            SoLoud::Wav* sound_health;
            SoLoud::Wav* sound_radius;
            SoLoud::Wav* sound_explosion;

	};
}

#endif // !BOXER