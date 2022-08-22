#include "Ability_Blink.h"

Application::Abilities::Blink::Blink(Player* player) : Ability(player, 3.0) {};

void Application::Abilities::Blink::Activate(float time)
{
	Ability::Activate(time);
	if (!GetActive())
		return;
	glm::vec2 dir = glm::normalize(GetPlayer()->GetMovementInput());
	if (glm::length(dir) > 0.0) {
		auto newloc = GetPlayer()->GetLocation() + (dir * 96.0f);
		newloc.x = Utils::clamp(newloc.x,17, 4096-17*2);
		newloc.y = Utils::clamp(newloc.y, 17, 1024-17*2);
		GetPlayer()->SetLocation(newloc);
		this->GetCave()->SetBlock(glm::round(newloc / glm::vec2(16)),cave_tile::empty);
	} else {
		this->Reset();
		return;
	}
}

void Application::Abilities::Blink::Deactivate(float time)
{
	Ability::Deactivate(time);
}

void Application::Abilities::Blink::Update(float time)
{
	Ability::Update(time);

}
