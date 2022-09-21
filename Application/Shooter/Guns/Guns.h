#ifndef Guns
#define Guns

#include "../Player/Player.h"
#include "../Cave/Cave.h"
#include <GLM/glm.hpp>

namespace Application {
	class Gun {
	public:
		Gun(float reload_duration,int max_ammo,float spread,float range, float bullets_per_shot,float zoom_blocks, float penetration, float damage, float rate) {
			this->reload_duration = reload_duration; this->max_ammo = max_ammo; this->spread = spread; this->range = range; this->bullets_per_shot = bullets_per_shot; this->zoom_blocks = zoom_blocks; this->penetration = penetration; this->damage = damage; this->fire_rate = rate;
		};
		Gun(float reload_duration, int max_ammo, float spread, float range, float bullets_per_shot, float zoom_blocks, float penetration, float damage, float rate,float bullet_size) {
			this->reload_duration = reload_duration; this->max_ammo = max_ammo; this->spread = spread; this->range = range; this->bullets_per_shot = bullets_per_shot; this->zoom_blocks = zoom_blocks; this->penetration = penetration; this->damage = damage; this->fire_rate = rate;this->tolerance = bullet_size;
		};
		bool Fire(float time, Cave* cave, Player* other, glm::vec3 transform = glm::vec3(-1,-1,-1)); // returns other's health or -1 if missed
		auto GetZoom() { return zoom_blocks; };
		auto GetAmmo() { return current_ammo; };
		void Aim(float angle) { this->angle = std::round(angle); };

		void SetLocation(glm::vec2 location) { this->location = location; };
		auto GetAngle() { return angle; };
		void UpdateCooldown(float time) { this->cooldown = fire_rate - (time - last_fire);};
	private:
		float reload_duration = 0;
		int max_ammo = 0;
		int current_ammo = max_ammo;
		float spread = 0; // DEGREES!
		float range = 0; // PIXELS
		float bullets_per_shot = 0;
		float zoom_blocks = 0;
		float penetration = 0;
		float damage = 0.0;
		float fire_rate = 0.0; // seconds

		float cooldown = 0.0;
		float start_reload_time = 0.0;
		float angle = 0.0;
		glm::vec2 location = glm::vec2(0);

		float tolerance = 5; // PIXELS  BULLET SIZE
		float last_fire = 0.0;
	};
}

#define TEST_GUN Gun(0, 0, 0, 100, 1, 32, 3, 10,0.25);
#define SNIPER_RIFLE Gun(0,0,5,500,1,128,5,75,1.0);
#define SHOTGUN Gun(0,0,45,100,8,15,1,10,1.0);
#define SMG Gun(0,0,20,200,1,50,0,10,0.1);
#define ASSAULT_RIFLE Gun(0,0,10,300,1,100,1,35,0.3);
#define KATANA Gun(0,0,0,24,1,0,1,150,0.8,18);
#endif // !Guns