#include "Ability_Teleport.h"

Application::Abilities::Teleport::Teleport(Player* player) : Ability(player, 3.0) {};

void Application::Abilities::Teleport::Activate(float time)
{
	Ability::Activate(time);
	if (!GetActive())
		return;
		
	glm::vec2 newloc = GetPlayer()->GetCursrorPosition();
	newloc.x = Utils::clamp(newloc.x, 17, 4096 - 17 * 2);
	newloc.y = Utils::clamp(newloc.y, 17, 1024 - 17 * 2);
	GetPlayer()->SetLocation(glm::round(newloc / glm::vec2(16)) * 16.0f);
	this->GetCave()->SetBlock(glm::round(newloc / glm::vec2(16)), cave_tile::empty);	
}

void Application::Abilities::Teleport::Deactivate(float time)
{
	Ability::Deactivate(time);
}

void Application::Abilities::Teleport::Update(float time)
{
	Ability::Update(time);

}
