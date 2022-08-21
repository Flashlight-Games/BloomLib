#ifndef ABILITY
#define ABILITY

#include "../Player/Player.h"

namespace Application {
	class Ability
	{
	public:
		Ability(Player* player, float recharge_speed);

		virtual void Update(float time);
		virtual void Activate(float time);
		virtual void Deactivate(float time);

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