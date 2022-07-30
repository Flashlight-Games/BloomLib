#pragma once
#include <GLM/glm.hpp>
#include "kinematicBody/KinematicBody.h"
#include "sprite/sprite.h"

namespace Application {
	class Player
	{
	public:
		Player(glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix);
		void Move(float DeltaTime, glm::vec2 movement) { this->movement_input = movement * (movement_speed * (DeltaTime / (1000/60)) * mass); kinematics->ApplyForce(movement_input); };
		void Aim(float angle);
		void Jump() { if (jump_timer > 0) { kinematics->ApplyForce(glm::vec2(0, 500)); jump_timer = 0.0; reset_jump = false; } };
		void Shoot();
		void Special();
		void ResetJump() { reset_jump = true; if (kinematics->GetVelocity().y > 0 && !landed) { kinematics->ApplyVelocity(-kinematics->GetVelocity() / glm::vec2(2.0) * glm::vec2(0,1)); } };

		auto getvel() { return kinematics->GetSpeed(); };
		auto GetHue() { return hue; };

		void SetVelocity(glm::vec2 velocity) { kinematics->SetVelocity(velocity); };
		auto GetVelocity() { return kinematics->GetVelocity(); };

		void Update(double DeltaTime, std::vector<PhysX::collisionBox*> others);
		void Draw(glm::mat4 view_matrix) { this->sprite->GetShader()->use(); this->sprite->GetShader()->uniformVec4(color_uid,glm::vec4(color,1)); ; this->sprite->Draw(view_matrix); };
		auto GetLocation() { return sprite->GetLocation(); };
		void SetLocation(glm::vec2 location) { kinematics->SetLocation(location); };
		void Damage(float damage) { health -= damage; };
		auto Getdepth(std::vector<PhysX::collisionBox*> others) { return kinematics->GetDepth(others); };

		void SetHue(float hue) { this->hue = hue; this->color = Utils::HSVtoRGB(glm::vec3(hue, 100, 100)); };
	private:
		glm::vec2 movement_input = glm::vec2(0);
		uint8_t health = 100;

		PhysX::KinematicBody* kinematics;
		Bloom::Sprite* sprite;
		float movement_speed = 0.5;
		float mass = 30;
		
		float coyote_time = 0.33;
		float jump_timer = coyote_time;
		bool landed = false;
		bool reset_jump = true;

		int color_uid;
		int hue = 0;
		glm::vec3 color;
	};
}

