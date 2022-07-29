#pragma once
#include <GLM/glm.hpp>
#include "kinematicBody/KinematicBody.h"
#include "sprite/sprite.h"

namespace Application {
	class Player
	{
	public:
		Player(glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix);
		void Move(glm::vec2 movement) { this->movement_input = movement * (movement_speed*mass); kinematics->ApplyForce(movement_input);};
		void Aim(float angle);
		void Jump() { if (jump_timer > 0) { kinematics->ApplyForce(glm::vec2(0, 500)); jump_timer = 0.0; reset_jump = false; } };
		void Shoot();
		void Special();
		void ResetJump() { reset_jump = true; if (kinematics->GetVelocity().y > 0 && !landed) { kinematics->ApplyVelocity(-kinematics->GetVelocity() / glm::vec2(2.0) * glm::vec2(0,1)); } };

		auto getvel() { return kinematics->GetSpeed(); };
		
		void Update(double DeltaTime, std::vector<PhysX::collisionBox*> others);
		void Draw(glm::mat4 view_matrix) { this->sprite->Draw(view_matrix); };
		auto GetLocation() { return sprite->GetLocation(); };
		void Damage(float damage) { health -= damage; };
		auto Getdepth(std::vector<PhysX::collisionBox*> others) { return kinematics->GetDepth(others); };
	private:
		glm::vec2 movement_input = glm::vec2(0);
		uint8_t health = 100;

		PhysX::KinematicBody* kinematics;
		Bloom::Sprite* sprite;
		float movement_speed = 0.5;
		float mass = 30;
		
		float coyote_time = 1.0;
		float jump_timer = coyote_time;
		bool landed = false;
		bool reset_jump = true;
	};
}

