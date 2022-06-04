#ifndef PLAYER
#define PLAYER

#include "../src/dependencies.h"

namespace Application{
	class Player {
	public:
		Player(Game::Engine* app);
		void Draw(Game::Engine* app);
		void SetPosition(Game::Engine* app,int radius);

		glm::vec2 GetPosition() { return position; };

		int GetVelocity() { return (int)glm::length(position - past_positions[0])*10; };

	private:
		Bloom::Sprite* box;
		glm::vec2 position = glm::vec2(500,500);

		glm::vec2 past_positions[10];

		glm::vec3 playerColor = glm::vec3(0, 0.75, 1);
		glm::vec3 trailColor = glm::vec3(0, 0.75, 1);
	};
}
#endif