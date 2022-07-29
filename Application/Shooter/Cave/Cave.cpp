#include "Cave.h"

using namespace Application;

Cave::Cave(glm::mat4 view_matrix,glm::mat4 pixel_projection_matrix, int seed)
{
	tile_sprite = new Bloom::Sprite("res/engine/textures/tilemap.png", 1, view_matrix, pixel_projection_matrix,glm::vec2(0.5),glm::vec2(1.0),glm::vec2(1.0/4.0),glm::vec2(0.0));
	tile_sprite->SetShader(new Bloom::Shader("res/shooter/shaders/cave/caveshader_vertex.glsl", "res/engine/shaders/sprite/spriteshader_fragment.glsl"), view_matrix, pixel_projection_matrix);

	if (seed == -1)
		seed = Utils::RandFloat(0,2400000);

	float fill =  Utils::map(Utils::Noise_Simplex(glm::vec2( - 1.0), seed),-1,1,0.35,0.45);
	std::cout << fill << "\n";
	//NOISE
	for (int x = 0; x < size.x; x++)
	{
		std::vector<cave_tile> row;
		
		for (int y = 0; y < size.y; y++)
		{
			bool block = (((Utils::Noise_Simplex(glm::vec2(x, y), seed) + 1) / 2.0) <= fill);
			row.push_back((block) ? cave_tile::block : cave_tile::empty);
		}
		tiles.push_back(row);
		
	}

	//CELLULAR AUTOMATA
	for (int i = 0; i < iterations; i++)
	{
		for (int x = 0; x < size.x; x++)
		{
			for (int y = 0; y < size.y; y++)
			{
				if (x == 0 || x == size.x - 1 || y == 0 || y == size.y - 1)
					tiles.at(x).at(y) = cave_tile::block;
				else {
					int sum = tiles.at(x).at(y) == cave_tile::block;
					sum += tiles.at(x + 1).at(y + 0) == cave_tile::block;
					sum += tiles.at(x - 1).at(y - 0) == cave_tile::block;
					sum += tiles.at(x + 0).at(y + 1) == cave_tile::block;
					sum += tiles.at(x - 0).at(y - 1) == cave_tile::block;
					sum += tiles.at(x + 1).at(y + 1) == cave_tile::block;
					sum += tiles.at(x - 1).at(y - 1) == cave_tile::block;
					sum += tiles.at(x + 1).at(y - 1) == cave_tile::block;
					sum += tiles.at(x + 1).at(y + 1) == cave_tile::block;


					if ((sum >= 4 && tiles.at(x).at(y) == cave_tile::block) || sum >= 5)
						tiles.at(x).at(y) = cave_tile::block;
					else
						tiles.at(x).at(y) = cave_tile::empty;
				}
			}
		}
	}

	//TEXTURES
	for (int x = 0; x < size.x; x++)
	{
		std::vector<uint8> texture_row;
		for (int y = 0; y < size.y; y++)
		{
			if (tiles.at(Utils::clamp(x, 0, tiles.size()-1)).at(Utils::clamp(y+1,0, tiles.at(x).size()-1)) != cave_tile::block && tiles.at(Utils::clamp(x, 0, tiles.size()-1)).at(Utils::clamp(y - 1,0, tiles.at(x).size()-1)) != cave_tile::block) {
				if (tiles.at(Utils::clamp(x + 1, 0, tiles.size()-1)).at(Utils::clamp(y, 0, tiles.at(x).size()-1)) != cave_tile::block && tiles.at(Utils::clamp(x - 1, 0, tiles.size()-1)).at(Utils::clamp(y, 0, tiles.at(x).size()-1)) != cave_tile::block)
					texture_row.push_back(16);
				else if (tiles.at(Utils::clamp(x + 1, 0, tiles.size()-1)).at(Utils::clamp(y, 0, tiles.at(x).size()-1)) == cave_tile::block && tiles.at(Utils::clamp(x - 1, 0, tiles.size()-1)).at(Utils::clamp(y, 0, tiles.at(x).size()-1)) != cave_tile::block)
					texture_row.push_back(1);
				else if (tiles.at(Utils::clamp(x - 1, 0, tiles.size()-1)).at(Utils::clamp(y, 0, tiles.at(x).size()-1)) == cave_tile::block && tiles.at(Utils::clamp(x + 1, 0, tiles.size()-1)).at(Utils::clamp(y, 0, tiles.at(x).size()-1)) != cave_tile::block)
					texture_row.push_back(3);
				else
					texture_row.push_back(2);
			}
			else if (tiles.at(Utils::clamp(x+1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block && tiles.at(Utils::clamp(x-1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block) {
				if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y+1, 0, tiles.at(x).size() - 1)) == cave_tile::block && tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y-1, 0, tiles.at(x).size() - 1)) != cave_tile::block)
					texture_row.push_back(4);
				else if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y-1, 0, tiles.at(x).size() - 1)) == cave_tile::block && tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y+1, 0, tiles.at(x).size() - 1)) != cave_tile::block)
					texture_row.push_back(12);
				else
					texture_row.push_back(8);
			}
			else if (tiles.at(Utils::clamp(x, 0, tiles.size() - 1)).at(Utils::clamp(y + 1, 0, tiles.at(x).size() - 1)) != cave_tile::block) {
				if(tiles.at(Utils::clamp(x-1, 0, tiles.size() - 1)).at(Utils::clamp(y, 0, tiles.at(x).size() - 1)) != cave_tile::block)
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
}

void Application::Cave::Draw(glm::mat4 view_matrix, glm::vec2 player_position, glm::vec2 screen_size)
{
	int counter = 0;
	tile_sprite->GetShader()->use();
	for (int x = 0; x < size.x; x++)
	{
		for (int y = 0; y < size.y; y++)
		{
			glm::vec2 location = glm::vec2(tile_size * x, tile_size * y);
			if (location.x > player_position.x - screen_size.x && location.x < player_position.x + screen_size.x && location.y > player_position.y - screen_size.y && location.y < player_position.y + screen_size.y) {
				if (tiles.at(x).at(y) == cave_tile::block) {
					glm::mat4 model_matrix = glm::mat4(1.0);
					model_matrix = glm::translate(model_matrix, glm::vec3(location, 0.0f));
					model_matrix = glm::scale(model_matrix, glm::vec3(0.25, 0.25, 1.0));

					tile_sprite->GetShader()->uniformMat4(tile_sprite->GetShader()->getUniformId((std::string("model[") + std::to_string(counter) + std::string("]")).c_str()), model_matrix);
					glm::vec2 tpos = glm::vec2(0);
					int index = tile_textures.at(x).at(y);
					tpos.x = (index-1) % 4;
					tpos.y = std::floor((index-1) / 4.0);
					
					tile_sprite->GetShader()->uniformVec2(tile_sprite->GetShader()->getUniformId((std::string("texuv[") + std::to_string(counter) + std::string("]")).c_str()), glm::vec2(0.25,0.25)*tpos);
					counter++;
				}
				

				if (counter >= 100) {
					tile_sprite->DrawInstanced(view_matrix, 100);
					counter = 0;
				}
			}
		}
	}
	tile_sprite->DrawInstanced(view_matrix, counter);
}
