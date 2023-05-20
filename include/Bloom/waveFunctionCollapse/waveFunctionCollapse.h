#ifndef WAVEFUNCTIONCOLLAPSE
#define WAVEFUNCTIONCOLLAPSE

#include <vector>
#include "GLM/glm.hpp"
#include <iostream>
#include "../utils/utils.h"

namespace Bloom {
	namespace WFC {
		struct Tile {
			Tile(int max_val) {
				for (int i = 0; i <= max_val; i++)
				{
					this->superposition.push_back(i);
				}
			}
			void Print();
			int GetEntropy() { return this->superposition.size(); };
			bool Contains(int contains);
			std::vector<int> superposition;
		};

		class Pattern {
		public:
			Pattern(std::vector<std::vector<int>> pattern);
			bool Match(std::vector<std::vector<Bloom::WFC::Tile>> target);
			int MaxVal();
			glm::vec2 GetSize();
			int GetCenter();
		private:
			std::vector<std::vector<int>> pattern;
		};

		class WaveFunctionCollapse {
		public:
			WaveFunctionCollapse(glm::vec2 size, std::vector<Pattern> patterns);
			~WaveFunctionCollapse();
			void Print();
			std::vector<std::vector<Tile>> GetSubset(glm::vec2 center, glm::vec2 size);
		private:
			void InitGrid(std::vector<Pattern> patterns);
			glm::vec2 size;
			std::vector<std::vector<Tile>> grid;
			int max_val = 0;
		};
	
		std::vector<Bloom::WFC::Pattern> findPatterns(std::vector <std::vector<int>> bitmap,glm::vec2 pattern_size = glm::vec3(3));
	}
}

#endif