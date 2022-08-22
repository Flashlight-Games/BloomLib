#include "Ability.h"

Application::Ability::Ability(Player* player, float recharge_speed)
{
	this->player = player; this->recharge_speed = recharge_speed;
}

void Application::Ability::Update(float time)
{
	this->cooldown = recharge_speed - (time - last_active); this->active_duration = time - activate_time;
}

void Application::Ability::Activate(float time)
{
	if (this->cooldown <= 0.0) { this->active = true; activate_time = time; }
}

void Application::Ability::Deactivate(float time)
{
	if (this->active) { this->active = false;  this->last_active = time; this->active_duration = 0.0; } 
}
void Application::Ability::Reset()
{
	this->cooldown = 0.0;
}
void Application::Ability::SetCave(Cave* cave)
{
	this->cave = cave;
}

