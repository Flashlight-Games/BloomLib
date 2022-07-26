#pragma once
#include <GLM/glm.hpp>
#include <vector>
#include "sprite/sprite.h"
#include "utils/utils.h"

namespace Application {
	enum cave_tile{empty,block};
	class Cave {
	public:
		Cave(glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix, int seed = -1);
		void Draw(glm::mat4 view_matrix, glm::vec2 player_position, glm::vec2 screen_size);

	private:
		glm::vec2 size = glm::vec2(256,64);
		int tile_size = 16;
		int iterations = 4;

		std::vector<std::vector<cave_tile>> tiles;
		std::vector<std::vector<uint8>> tile_textures;
		Bloom::Sprite* tile_sprite;
	};
}