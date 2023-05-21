#include "waveFunctionCollapse.h"
#include <random>

Bloom::WFC::WaveFunctionCollapse::WaveFunctionCollapse(glm::vec2 size, std::vector<Pattern> patterns)
{
	// INITIALIZATION
	this->size = size;
	this->max_val = 0;
	std::cout << "WFC INIT!\n";
	this->pattern_size = glm::vec2(patterns.at(0).GetSize());

	this->patterns = patterns;
	shufflePatterns();

	InitGrid(patterns);
	Print(this->grid);
	auto originalGrid = this->grid;

	bool success = false;
	while (!success) {
		//Print(this->grid);
		contradictions++;
		std::cout << "FAIL\n";
		this->grid = originalGrid;
		success = Collapse(this->grid);
	}

	std::cout << "\n\nFINAL GRID:\n";
	this->Print(this->grid);
}

void Bloom::WFC::WaveFunctionCollapse::Print(std::vector<std::vector<Tile>> grid)
{
	for (int y = 0; y < this->size.y; y++)
	{
		for (int x = 0; x < this->size.x; x++)
		{
			grid.at(y).at(x).Print();
			std::cout << ", ";
		}
		std::cout << "\n";
	}
}

std::vector<std::vector<Bloom::WFC::Tile>> Bloom::WFC::WaveFunctionCollapse::GetSubset(std::vector<std::vector<Tile>> grid, glm::vec2 center, glm::vec2 size)
{
	std::vector<std::vector<Tile>> retval;
	for (int y = -size.y / 2; y <= size.y / 2; y++)
	{
		auto row = std::vector<Tile>();
		for (int x = -size.x / 2; x <= size.x / 2; x++)
		{
			if (x + center.x < 0 || y + center.y < 0 || x + center.x >= this->size.x || y + center.y >= this->size.y) {
				row.push_back(Tile(this->max_val));
			} else
				row.push_back(grid.at(y + center.y).at(x + center.x));
		}
		retval.push_back(row);
	}
	return retval;
}

bool Bloom::WFC::WaveFunctionCollapse::Collapse(std::vector<std::vector<Tile>>& grid) {
	if (isGridDone(grid)) {
		this->grid = grid;
		return true;
	}

	auto idx = leastEntropyTileIdx(grid);
	auto& cell = grid[idx.y][idx.x];
	//Print(grid);
	//std::cout << "TILE (" << idx.x << "," << idx.y << ") MATCHES:\n";
	shufflePatterns();
	for (const auto& option : cell.superposition)
	{
		const auto& subset = GetSubset(grid, idx, this->pattern_size);

		for (const auto& pattern : this->patterns)
		{
			auto patternCopy = pattern;  // Create a non-const copy
			if (patternCopy.Match(subset)) {
				grid[idx.y][idx.x].superposition = std::vector<int>({ patternCopy.GetCenter() });
				auto res = Collapse(grid);
				if (res)
					return res;
			}
		}
	}
	//std::cout << "\n";
	return false;
}

void Bloom::WFC::WaveFunctionCollapse::shufflePatterns()
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto rng = std::default_random_engine{ seed };
	std::shuffle(std::begin(this->patterns), std::end(this->patterns), rng);
}

void Bloom::WFC::WaveFunctionCollapse::InitGrid(std::vector<Pattern> patterns)
{
	this->grid.clear();
	for (int i = 0; i < patterns.size(); i++)
	{
		this->max_val = std::max(patterns.at(i).MaxVal(), this->max_val);
	}
	for (int y = 0; y < this->size.y; y++)
	{
		this->grid.push_back(std::vector<Tile>());
		for (int x = 0; x < this->size.x; x++)
		{
			this->grid.at(y).push_back(Tile(this->max_val));
		}
	}
}

bool Bloom::WFC::WaveFunctionCollapse::isGridDone(const std::vector<std::vector<Tile>>& grid)
{
	// FIND LEAST ENTROPY TILE
	for (int y = 0; y < this->size.y; y++)
	{
		for (int x = 0; x < this->size.x; x++)
		{
			if (grid.at(y).at(x).superposition.size()> 1) {
				return false;
			}
		}
	}

	return true;
}

glm::vec2 Bloom::WFC::WaveFunctionCollapse::leastEntropyTileIdx(const std::vector<std::vector<Tile>>& grid) const {
	int least_entropy_value = 9999;
	glm::vec2 least_entropy_idx = glm::vec2(-1);

	// FIND LEAST ENTROPY TILE
	for (int y = 0; y < this->size.y; y++) {
		for (int x = 0; x < this->size.x; x++) {
			if (grid.at(y).at(x).superposition.size() < least_entropy_value && grid.at(y).at(x).superposition.size() > 1) {
				least_entropy_value = grid.at(y).at(x).superposition.size();
				least_entropy_idx = glm::vec2(x, y);
			}
		}
	}

	return least_entropy_idx;
}

Bloom::WFC::Pattern::Pattern(std::vector<std::vector<int>> pattern)
{
	for (int i = 0; i < pattern.size(); i++)
	{
		if (pattern.at(i).size() != pattern.at(0).size()) {
			std::cout << "NOT ALL PATTERN ROWS ARE THE SAME SIZE!\n";
			exit(-1);
		}
	}
	this->pattern = pattern;
}

bool Bloom::WFC::Pattern::Match(const std::vector<std::vector<Bloom::WFC::Tile>>& target) const {
	if (glm::vec2(target.at(0).size(), target.size()) != glm::vec2(pattern.at(0).size(), pattern.size())) {
		throw std::runtime_error("PATTERN SIZE NOT EQUAL TO TARGET SIZE!");
	}

	// MATCH VALUES
	for (int y = 0; y < this->pattern.size(); y++)
	{
		for (int x = 0; x < this->pattern.at(y).size(); x++)
		{
			auto& currentTarget = target.at(y).at(x);
			if (std::find(currentTarget.superposition.begin(), currentTarget.superposition.end(), this->pattern.at(y).at(x)) == currentTarget.superposition.end()) {
				return false;
			}
		}
	}
	return true;
}

int Bloom::WFC::Pattern::MaxVal()
{
	int maxval = 0;
	for (int i = 0; i < pattern.size(); i++)
	{
		for (int j = 0; j < pattern.at(i).size(); j++)
		{
			maxval = std::max(pattern.at(i).at(j), maxval);
		}
	}
	return maxval;
}

glm::vec2 Bloom::WFC::Pattern::GetSize()
{
	return glm::vec2(this->pattern.size(), this->pattern[0].size());
}

int Bloom::WFC::Pattern::GetCenter()
{
	auto center = this->pattern.at(this->pattern.size() / 2).at(this->pattern.at(this->pattern.size() / 2).size() / 2);
	return center;
}

void Bloom::WFC::Pattern::Print()
{
	// PRINT PATTERN
	for (int y = 0; y < this->pattern.size(); y++)
	{
		for (int x = 0; x < this->pattern.at(y).size(); x++)
		{
			std::cout << this->pattern.at(y).at(x) << ",";
		}
		std::cout << "\n";
	}
}

void Bloom::WFC::Tile::Print()
{
	std::cout << "(";
	for (int i = 0; i < superposition.size(); i++)
	{
		std::cout << superposition.at(i) << ",";
	}
	std::cout << ")";
}

bool Bloom::WFC::Tile::Contains(int contains)
{
	for (int i = 0; i < this->superposition.size(); i++)
	{
		if (this->superposition.at(i) == contains)
			return true;
	}
	return false;
}

std::vector<Bloom::WFC::Pattern> Bloom::WFC::findPatterns(std::vector<std::vector<int>> bitmap, glm::vec2 pattern_size)
{
	std::vector<Pattern> patterns;
	int counter = 0;
	for (int i = pattern_size.y/2; i < bitmap.size() - (pattern_size.y - pattern_size.y/2); i++)
	{
		for (int j = pattern_size.x / 2; j < bitmap.at(i).size() - (pattern_size.x - pattern_size.x / 2); j++)
		{
			counter++;
			std::cout << counter << " " << i << " " << j << "\n";
			std::vector<std::vector<int>> pattern;

			for (int y = -pattern_size.y / 2; y <= pattern_size.y / 2; y++)
			{
				auto row = std::vector<int>();
				for (int x = -pattern_size.x / 2; x <= pattern_size.x / 2; x++)
				{
					row.push_back(bitmap.at(y + i).at(x + j));
				}
				pattern.push_back(row);
			}
			patterns.push_back(pattern);

			for (int y = 0; y < pattern.size(); y++)
			{
				for (int x = 0; x < pattern.at(y).size(); x++)
				{
					std::cout << pattern.at(y).at(x) << ",";
				}
				std::cout << "\n";
			}
			std::cout << "\n";
		}
	}
	return patterns;
}

std::vector<std::vector<int>> Bloom::WFC::CreateBitmapFromImage(const char* source)
{
	int width, height, channels;
	unsigned char* image = stbi_load(source, &width, &height, &channels, 0);

	if (!image) {
		std::cout << "Failed to load image: " << source << std::endl;
		exit(-1);
	}

	std::vector<std::vector<int>> bitmap(height, std::vector<int>(width));

	// Convert image data to bitmap
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int pixelIndex = (y * width + x) * channels;

			// Assuming grayscale image (channels = 1)
			unsigned char pixelValue = image[pixelIndex];

			// Assign pixel value to the corresponding position in the bitmap
			bitmap[y][x] = static_cast<int>(pixelValue);
		}
	}

	stbi_image_free(image);

	return bitmap;
}