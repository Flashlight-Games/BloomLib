#ifndef ENEMY
#define ENEMY

#include "../src/dependencies.h"

namespace Application{
	class Enemy {
	public:
		Enemy(Game::Engine* app,glm::vec2 position);
		void Draw(Game::Engine* app);
		void SetPosition(Game::Engine* app);

		glm::vec2 GetPosition() { return position; };
	private:
		Bloom::Sprite* box;
		glm::vec2 position = glm::vec2(500,500);
		glm::vec2 velocity = glm::vec2(0, 0);
		glm::vec3 enemyColor = glm::vec3(1, 0.5, 0);

		float speed = 0.5;
		float acceleration = 0.1;
	};
}
#endif