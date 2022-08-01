#pragma once
#include <GLM/glm.hpp>
#include <vector>
#include "sprite/sprite.h"
#include "utils/utils.h"

namespace Application {
	enum cave_tile{empty,block};
	class Cave {
	public:
		Cave(glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix);
		void Draw(glm::mat4 view_matrix, glm::vec2 player_position, glm::vec2 screen_size);
		std::vector<std::vector<cave_tile>> GetTiles() { return tiles; };
		void SetBlock(glm::vec2 position, cave_tile new_tile) { 
			position.x = Utils::clamp(position.x, 1, tiles.size()-1);
			position.y = Utils::clamp(position.y, 1, tiles.at(position.x).size() - 1);
			tiles.at(position.x).at(position.y) = new_tile;

			//TEXTURES
			tile_textures.clear();
			for (int x = 0; x < size.x; x++)
			{
				std::vector<uint8> texture_row;
				for (int y = 0; y < size.y; y++)
				{
					if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y + 1, 0, tiles.at(x).size() - 1)) != cave_tile::block && tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y - 1, 0, tiles.at(x).size() - 1)) != cave_tile::block) {
						if (tiles.at(Utils::clamp(x + 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block && tiles.at(Utils::clamp(x - 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(16);
						else if (tiles.at(Utils::clamp(x + 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) == cave_tile::block && tiles.at(Utils::clamp(x - 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(1);
						else if (tiles.at(Utils::clamp(x - 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) == cave_tile::block && tiles.at(Utils::clamp(x + 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(3);
						else
							texture_row.push_back(2);
					}
					else if (tiles.at(Utils::clamp(x + 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block && tiles.at(Utils::clamp(x - 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block) {
						if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y + 1, 0, tiles.at(x).size() - 1)) == cave_tile::block && tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y - 1, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(4);
						else if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y - 1, 0, tiles.at(x).size() - 1)) == cave_tile::block && tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y + 1, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(12);
						else
							texture_row.push_back(8);
					}
					else if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y + 1, 0, tiles.at(x).size() - 1)) != cave_tile::block) {
						if (tiles.at(Utils::clamp(x - 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(13);
						else if (tiles.at(Utils::clamp(x + 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(15);
						else
							texture_row.push_back(14);
					}
					else if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y - 1, 0, tiles.at(x).size() - 1)) != cave_tile::block) {
						if (tiles.at(Utils::clamp(x - 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(5);
						else if (tiles.at(Utils::clamp(x + 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(7);
						else
							texture_row.push_back(6);
					}
					else {
						if (tiles.at(Utils::clamp(x - 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(9);
						else if (tiles.at(Utils::clamp(x + 1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
							texture_row.push_back(11);
						else
							texture_row.push_back(10);
					}

				}
				tile_textures.push_back(texture_row);
			}
			
		};
		int Generate(int seed);
	private:
		glm::vec2 size = glm::vec2(256,64);
		int tile_size = 16;
		int iterations = 4;

		std::vector<std::vector<cave_tile>> tiles;
		std::vector<std::vector<uint8>> tile_textures;
		Bloom::Sprite* tile_sprite;
	};
}