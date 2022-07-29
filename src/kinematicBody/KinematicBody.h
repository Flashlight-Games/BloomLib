#pragma once
#include "collision/collision.h"
#include "sprite/sprite.h"

namespace PhysX {
	class KinematicBody
	{
	public:
		KinematicBody(std::string channel,float mass,Bloom::Sprite* source,glm::vec2 gravity = glm::vec2(0, - 9.8),float bounce = 0.05f, float drag = 100.0f);
		glm::vec2 GetLocation(float DeltaTime, std::vector<PhysX::collisionBox*> other_boxes); // CLEAR FORCES AND GET LOCATION
		void ApplyForce(glm::vec2 force) { applied_forces.push_back(force); };
		void ApplyVelocity(glm::vec2 velocity) { this->velocity += velocity; };
		auto GetVelocity() { return velocity; };

		glm::vec2 GetDepth(std::vector<PhysX::collisionBox*> other_boxes);
		auto GetSpeed() { return glm::length(velocity); };
	private:
		PhysX::collisionBox* collision;
		glm::vec2 location = glm::vec2(500,500);
		glm::vec2 net_force = glm::vec2(0);
		glm::vec2 velocity = glm::vec2(0,0);
		glm::vec2 acceleration = glm::vec2(0);
		std::vector<glm::vec2> applied_forces;
		bool hit = false;

		float mass;
		float pixels_to_meters = 25;
		glm::vec2 gravity;
		float bounciness = 0.05;
		float drag = 100.0f;
	};
}