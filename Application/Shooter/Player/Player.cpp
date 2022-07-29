#include "Player.h"

Application::Player::Player(glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix)
{
	this->sprite = new Bloom::Sprite("res/engine/textures/rect16.png", 1, view_matrix, pixel_projection_matrix, glm::vec2(0.5,0.5));
	this->sprite->SetScale(glm::vec2(0.8));
	this->kinematics = new PhysX::KinematicBody("main", mass, this->sprite);
}

void Application::Player::Update(double DeltaTime, std::vector<PhysX::collisionBox*> others)
{
	this->sprite->SetLocation((this->kinematics->GetLocation(DeltaTime,others)));
	for (int i = 0; i < others.size(); i++)
	{
		bool landed = PhysX::LineTraceNormal("main", others.at(i), sprite->GetLocation() + glm::vec2((sprite->GetSize().x)/2.0-2.0, 0), sprite->GetLocation() + glm::vec2(sprite->GetSize().x/2-1, -9)) != glm::vec2(0) || PhysX::LineTraceNormal("main", others.at(i), sprite->GetLocation() + glm::vec2(-((sprite->GetSize().x) / 2.0 - 2.0), 0), sprite->GetLocation() + glm::vec2(-(sprite->GetSize().x / 2 - 1), -9)) != glm::vec2(0);
		if (jump_timer <= 0.0 && landed && reset_jump) {
			jump_timer = coyote_time;
		}
	}
	jump_timer -= DeltaTime / 1000;
}
