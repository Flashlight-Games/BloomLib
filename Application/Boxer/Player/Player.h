#ifndef PLAYER
#define PLAYER

#include "../src/dependencies.h"

namespace Application{
	class Player {
	public:
		Player(Game::Engine* app);
		void Draw(Game::Engine* app);
		void SetPosition(Game::Engine* app);

		glm::vec2 GetPosition() { return position; };

		int GetSpeed() { return (int)glm::length(position - past_positions[0])*10; };
		glm::vec2 GetVelocity() { return (position - past_positions[0]); };
		PhysX::collisionBox* GetCollisionBox() { return collision_box; };

		void SetMoveRadius(int radius) { move_radius = radius; };

	private:
		void Trail(Game::Engine* app);
		Bloom::Sprite* box;
		glm::vec2 position = glm::vec2(500,500);

		float speed = 5;
		glm::vec2 past_positions[10];

		glm::vec3 playerColor = glm::vec3(0, 0.75, 1);
		glm::vec3 trailColor = glm::vec3(0, 0.75, 1);

		PhysX::collisionBox* collision_box;

		std::thread* trail_thread;

		int move_radius = 0;
	};
}
#endif