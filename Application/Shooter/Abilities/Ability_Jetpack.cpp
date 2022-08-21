#include "Ability_Jetpack.h"

Application::Abilities::Jetpack::Jetpack(Player* player) : Ability(player, 2.0)
{
	Ability(player, 2.0);
}

void Application::Abilities::Jetpack::Activate(float time)
{
	Ability::Activate(time);
	if (!GetActive())
		return;
}

void Application::Abilities::Jetpack::Deactivate(float time)
{
	Ability::Deactivate(time);
}

void Application::Abilities::Jetpack::Update(float time)
{
	Ability::Update(time);
	if (GetActive()) {
		if (this->GetActiveDuration() < 1.0)
			GetPlayer()->GetKinematics()->ApplyForce(glm::vec2(0, 9.8));
		else
			Deactivate(time);
	}
}
