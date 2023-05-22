#ifndef WAVEFUNCTIONCOLLAPSE
#define WAVEFUNCTIONCOLLAPSE

#include <vector>
#include "GLM/glm.hpp"
#include <iostream>
#include "../utils/utils.h"
#include <STBI/stb_image.h>

namespace Bloom {
	namespace WFC {
		struct Tile {
			Tile(int max_val) {
				for (int i = 0; i <= max_val; i++)
				{
					this->superposition.push_back(i);
				}
			}
			void Print() const;
			int GetEntropy() { return this->superposition.size(); };
			bool Contains(int contains);
			std::vector<int> superposition;
		};

		class Pattern {
		public:
			Pattern(std::vector<std::vector<int>> pattern);
			bool Match(const std::vector<std::vector<Bloom::WFC::Tile>>& target) const;
			int MaxVal();
			glm::vec2 GetSize();
			int GetCenter();
			void Print();
			auto getPattern() const { return this->pattern; };
		private:
			std::vector<std::vector<int>> pattern;
		};

		class WaveFunctionCollapse {
		public:
			WaveFunctionCollapse(glm::vec2 size, std::vector<Pattern> patterns);
			~WaveFunctionCollapse();
			void Print(std::vector<std::vector<Tile>> grid,int depth = 0);
			auto getGrid() { return this->grid; };
			auto isReady() { return this->ready; };
		private:
			std::vector<std::vector<Tile>> GetSubset(std::vector<std::vector<Tile>> grid, glm::vec2 center, glm::vec2 size);
			bool RecursiveCollapse(std::vector<std::vector<Tile>>& grid, int depth = 0);
			bool Collapse(std::vector<std::vector<Tile>>& grid);
			bool Propagate(std::vector<std::vector<Tile>>& grid, glm::vec2 idx, int depth = 0);
			void shufflePatterns();
			void InitGrid(std::vector<Pattern> patterns);
			bool isGridDone(const std::vector<std::vector<Tile>>& grid);
			glm::vec2 leastEntropyTileIdx(const std::vector<std::vector<Tile>>& grid) const;
			glm::vec2 size;
			std::vector<std::vector<Tile>> grid;
			int max_val = 0;
			glm::vec2 pattern_size;
			std::vector<Pattern> patterns;
			int contradictions = 0;
			bool ready = false;
		};
	
		std::vector<Bloom::WFC::Pattern> findPatterns(std::vector <std::vector<int>> bitmap,glm::vec2 pattern_size = glm::vec3(3));
		std::vector<std::vector<int>> CreateBitmapFromImage(const char* source);
	}
}

#endif