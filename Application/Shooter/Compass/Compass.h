#ifndef COMPASS
#define COMPASS

#include "sprite/sprite.h"
#include "../Player/Player.h"

namespace Application {
	class Compass {
	public:
		Compass(Player* my_player, Player* other_player, glm::mat4 view_matrix, glm::mat4 projection_matrix);
		void Update();
		void Draw(glm::mat4 view_matrix);

	private:
		Bloom::Sprite* sprite;
		Player* my_player;
		Player* other_player;
	};
}

#endif
