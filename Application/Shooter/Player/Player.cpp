#include "Player.h"

Application::Player::Player(glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix)
{
	this->sprite = new Bloom::Sprite("res/engine/textures/rect16.png", 1, view_matrix, pixel_projection_matrix, glm::vec2(0.5,0.5));
	this->sprite->SetScale(glm::vec2(12.0/16.0));
	this->kinematics = new PhysX::KinematicBody("main", mass, this->sprite);

	this->sprite->GetShader()->use();
	this->color_uid = this->sprite->GetShader()->getUniformId("uColor");
}

void Application::Player::Jump()
{
	if (jump_timer > 0) {		
		if (kinematics->GetGravity().y < 0) {
			kinematics->ApplyForce(glm::vec2(0, 500)); jump_timer = 0.0; reset_jump = false;
		} else {
			kinematics->ApplyForce(glm::vec2(0, -500)); jump_timer = 0.0; reset_jump = false;
		}
	}
}

void Application::Player::Update(double DeltaTime, std::vector<PhysX::collisionBox*> others)
{
	for (int i = 0; i < others.size(); i++)
	{
		bool landed;
		if(kinematics->GetGravity().y < 0)
			landed = PhysX::LineTrace("main", others.at(i), sprite->GetLocation() + glm::vec2((sprite->GetSize().x) / 2.0 - 1.0, 0), sprite->GetLocation() + glm::vec2(sprite->GetSize().x / 2 - 1, -10)) != glm::vec2(INFINITY) || PhysX::LineTrace("main", others.at(i), sprite->GetLocation() + glm::vec2(-((sprite->GetSize().x) / 2.0 - 2.0), 0), sprite->GetLocation() + glm::vec2(-(sprite->GetSize().x / 2 - 1), -10)) != glm::vec2(INFINITY);
		else
			landed = PhysX::LineTrace("main", others.at(i), sprite->GetLocation() + glm::vec2((sprite->GetSize().x) / 2.0 - 1.0, 0), sprite->GetLocation() + glm::vec2(sprite->GetSize().x / 2 - 1, 10)) != glm::vec2(INFINITY) || PhysX::LineTrace("main", others.at(i), sprite->GetLocation() + glm::vec2(-((sprite->GetSize().x) / 2.0 - 2.0), 0), sprite->GetLocation() + glm::vec2(-(sprite->GetSize().x / 2 - 1), 10)) != glm::vec2(INFINITY);
		if (jump_timer <= 0.0 && landed && reset_jump) {
			jump_timer = coyote_time;
		}
		if (glm::abs(kinematics->GetVelocity()).y <= 0.1) {
			kinematics->SetDrag(200); // MORE DRAG ON GROUND
		} else {
			kinematics->SetDrag(100);
		}
	}
	jump_timer -= DeltaTime / 1000;

	this->sprite->SetLocation((this->kinematics->GetLocation(DeltaTime, others)));
}

void Application::Player::SetCursorPosition(glm::vec2 cursor_pos)
{
	this->cursor_position = cursor_pos;
}
