#include "Ability_GravityFlip.h"

Application::Abilities::GravityFlip::GravityFlip(Player* player) : Ability(player, 5.0) {

};

void Application::Abilities::GravityFlip::Activate(float time)
{
	Ability::Activate(time);
	if (!GetActive())
		return;
	GetPlayer()->GetKinematics()->SetGravity(GetPlayer()->GetKinematics()->GetGravity() * glm::vec2(1, -1));
}

void Application::Abilities::GravityFlip::Deactivate(float time)
{
	Ability::Deactivate(time);
}

void Application::Abilities::GravityFlip::Update(float time)
{
	Ability::Update(time);
}
