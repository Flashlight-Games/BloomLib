#ifndef APP
#define APP

#include "../../src/engine/engine.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "Upgrade Health/UpgradeHealth.h"
#include "Upgrade Radius/UpgradeRadius.h"

#include "Particle_Hit/particle_hit.h"
#include "Particle_Death/particle_death.h"
#include "Particle_Hurt/particle_hurt.h"

namespace Application{
	class Boxer : public Game::Engine {
        using Game::Engine::Engine;
        public:
            void Begin();
            void GameLoop();
            void UILoop();
            void InputLoop();

            bool GetInMenu() { return in_menu; };
            
            void ResetGame() {
                PlaySound(sound_menu);
                sound_music_id = PlaySoundBackground(sound_music, 0.2, true);
                transition_in_start_time = GetTime();
                EraseEnemies();
                emitter_hit->Clear();

                move_radius = start_radius;
                health = start_health;
                last_hp = health;
                in_menu = false;
                if (hitShakeX != nullptr && hitShakeY != nullptr) {
                    hitShakeY->Stop();
                    hitShakeX->Stop();
                }
                upgrade_health = nullptr;
                glfwSetTime(0);
                EraseEnemies();
            }

            void CloseGame(GLFWwindow* window) {
                glfwSetWindowShouldClose(window, true);
                PlaySound(sound_menu);
            }

            void PlayEmitterHIT() {
                auto particles = emitter_hit->Play(GetWindow(), GetTime(), player->GetPosition());
                for (size_t i = 0; i < particles.size(); i++)
                {
                    particles.at(i)->SetInitialVelocity(player->GetVelocity() * glm::vec2(((rand() % 10 / 10.0)+0.6) * 25, ((rand() % 10 / 10.0)+0.6) * 25));
                }
            }

            void PlayEmitterDEATH(Application::Enemy* enemy) {
                auto particles = emitter_death->Play(GetWindow(), GetTime());
                for (size_t i = 0; i < particles.size(); i++)
                {
                    particles.at(i)->SetLocation(enemy->GetPosition());
                }
            }

            void PlayEmitterHURT() {
                auto particles = emitter_hurt->Play(GetWindow(), GetTime());
                for (size_t i = 0; i < particles.size(); i++)
                {
                    particles.at(i)->SetLocation(protect->GetLocation());
                    particles.at(i)->SetColor(Utils::Lerp(glm::vec4(0.9, 0.97, 1, 1), glm::vec4(0, 0.75, 1, 1), (health / 3.0)));
                    particles.at(i)->SetInitialVelocity(glm::vec2(Utils::RandFloat(-1,1.0)*100, Utils::RandFloat(-1, 1.0) * 100));
                }
            }

            Application::Player* GetPlayer() { return player; };
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

            void UpdateEmitters();

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
            SoLoud::Wav* sound_menu;
            SoLoud::Wav* sound_music;

            int sound_music_id;

            Bloom::ParticleEmitter<Bloom::Particle_HIT>* emitter_hit;
            Bloom::ParticleEmitter<Bloom::Particle_DEATH>* emitter_death;
            Bloom::ParticleEmitter<Bloom::Particle_HURT>* emitter_hurt;
	};
}
#endif // !BOXER