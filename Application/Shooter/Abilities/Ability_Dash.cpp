#include "Ability_Dash.h"

Application::Abilities::Dash::Dash(Player* player) : Ability(player, 1.0) {};

void Application::Abilities::Dash::Activate(float time)
{
	Ability::Activate(time);
	if (!GetActive())
		return;
	glm::vec2 dir = glm::normalize(GetPlayer()->GetKinematics()->GetVelocity());
	GetPlayer()->GetKinematics()->SetVelocity(glm::vec2(0));
	GetPlayer()->GetKinematics()->ApplyForce(dir * glm::vec2(700));
	GetPlayer()->GetKinematics()->SetDrag(0);
}

void Application::Abilities::Dash::Deactivate(float time)
{
	Ability::Deactivate(time);
	if (GetActive()) {
		GetPlayer()->GetKinematics()->SetVelocity(glm::vec2(0));
		GetPlayer()->GetKinematics()->SetDrag(100);
	}
}

void Application::Abilities::Dash::Update(float time)
{
	Ability::Update(time);
	if (this->GetActive())
		if (this->GetActiveDuration() > 1)
			Deactivate(time);

}
