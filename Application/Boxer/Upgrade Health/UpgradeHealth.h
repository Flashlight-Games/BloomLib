#ifndef UPGRADEHEALTH
#define UPGRADEHEALTH

#include "../src/dependencies.h"

namespace Application{
	class Upgrade_Health {
	public:
		Upgrade_Health(Game::Engine* app,glm::vec2 position);
		void Draw(Game::Engine* app);
		void SetPosition(Game::Engine* app);

		glm::vec2 GetPosition() { return position; };
	private:
		Bloom::Sprite* box;
		glm::vec2 position = glm::vec2(500,500);
		glm::vec2 velocity = glm::vec2(0, 0);
		glm::vec3 upgradeColor = glm::vec3(0.2, 1, 0.2);

		float speed = 1.2;
		float acceleration = 0.04f;
	};
}
#endif