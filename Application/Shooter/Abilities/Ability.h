#ifndef ABILITY
#define ABILITY

#include "../Player/Player.h"

namespace Application {
	class Ability
	{
	public:
		Ability(Player* player, float recharge_speed) {
			this->player = player; this->recharge_speed = recharge_speed;
		}

		virtual void Update(float time) { this->cooldown = recharge_speed - (time - last_active); this->active_duration = time - activate_time; };
		virtual void Activate(float time) { if (this->cooldown <= 0.0) this->active = true; activate_time = time; };
		virtual void Deactivate(float time) { if (this->active) { this->active = false;  this->last_active = time; this->active_duration = 0.0; } };

		bool GetActive() { return this->active; };
		float GetActiveDuration() { return this->active_duration; };
		Player* GetPlayer() { return this->player; };
	private:
		float cooldown = 0.0;
		bool active = false;
		Player* player;
		float last_active = 0.0;
		float recharge_speed = 0.0;
		float active_duration = 0.0;
		float activate_time = 0.0;
	};
}
#endif