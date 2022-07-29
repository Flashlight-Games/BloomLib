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
		void Jump() { kinematics->ApplyForce(glm::vec2(0,500));};
		void Shoot();
		void Special();

		auto getvel() { return kinematics->GetVelocity(); };
		
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
	};
}

