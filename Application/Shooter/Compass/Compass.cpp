#include "Compass.h"

Application::Compass::Compass(Player* my_player, Player* other_player,glm::mat4 view_matrix, glm::mat4 projection_matrix)
{
	this->my_player = my_player; this->other_player = other_player;
	this->sprite = new Bloom::Sprite("res/engine/textures/rect16.png", 1, view_matrix, projection_matrix,glm::vec2(0.5),glm::vec2(0.33));
}

void Application::Compass::Update()
{
	this->sprite->SetLocation(Utils::PointAtAngleAndDistance(std::atan2(other_player->GetLocation().y - my_player->GetLocation().y, other_player->GetLocation().x - my_player->GetLocation().x),50) + my_player->GetLocation());
}

void Application::Compass::Draw(glm::mat4 view_matrix)
{
	if (Utils::Distance2D(my_player->GetLocation(), other_player->GetLocation()) > 125 && other_player->GetHealth() > 0.0) {
		sprite->GetShader()->use();
		sprite->GetShader()->uniformVec4(sprite->GetShader()->getUniformId("uColor"), glm::vec4(1, 0, 0,1));
		sprite->Draw(view_matrix);
	}
}
