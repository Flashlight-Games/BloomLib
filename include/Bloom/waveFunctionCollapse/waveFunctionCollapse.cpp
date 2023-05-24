#include "waveFunctionCollapse.h"
#include <random>
#include <chrono>

std::vector<int> Bloom::WFC::CalculateWeights(const std::vector<Pattern>& patterns) {
	// Count the frequency of each number appearing in the center of the patterns
	std::unordered_map<int, int> frequencyMap;
	for (const Pattern& pattern : patterns) {
		int centerNumber = pattern.GetCenter();
		frequencyMap[centerNumber]++;
	}
	// Create a vector of weights based on the frequencies
	std::vector<int> weights;
	for (const Pattern& pattern : patterns) {
		int centerNumber = pattern.GetCenter();
		weights.push_back(frequencyMap[centerNumber]);
	}
	return weights;
}

std::vector<int> Bloom::WFC::CalculateWeights(const std::vector<std::vector<int>>& bitmap) {
	// Count the frequency of each number appearing in the bitmap
	std::unordered_map<int, int> frequencyMap;
	for (const auto& row : bitmap) {
		for (int number : row) {
			frequencyMap[number]++;
		}
	}

	// Create a vector of weights based on the frequencies
	std::vector<int> weights;
	for (const auto& row : bitmap) {
		for (int number : row) {
			weights.push_back(frequencyMap[number]);
		}
	}

	return weights;
}

Bloom::WFC::WaveFunctionCollapse::WaveFunctionCollapse(glm::vec2 size, std::vector<Pattern> patterns)
{
	// INITIALIZATION
	this->size = size;
	this->max_val = 0;
	std::cout << "WFC INIT!\n";
	this->pattern_size = glm::vec2(patterns.at(0).GetSize());
	this->patterns = patterns;
	this->weights = CalculateWeights(this->patterns);
	InitGrid();

	Print();
}

Bloom::WFC::WaveFunctionCollapse::WaveFunctionCollapse(glm::vec2 size, std::vector<Pattern> patterns, std::vector<int> weights)
{
	// INITIALIZATION
	this->size = size;
	this->max_val = 0;
	std::cout << "WFC INIT!\n";
	this->pattern_size = glm::vec2(patterns.at(0).GetSize());
	this->patterns = patterns;
	this->weights = weights;
	InitGrid();

	Print();
}

void Bloom::WFC::WaveFunctionCollapse::Print(int depth)
{
	for (int y = 0; y < this->size.y; y++)
	{
		for (int i = 0; i < depth; i++)
		{
			std::cout << " ";
		}
		for (int x = 0; x < this->size.x; x++)
		{
			this->grid.at(y).at(x).Print();
			std::cout << ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
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

void Bloom::WFC::WaveFunctionCollapse::shufflePatterns()
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto rng = std::default_random_engine{ seed };
	std::shuffle(std::begin(this->patterns), std::end(this->patterns), rng);
}

void Bloom::WFC::WaveFunctionCollapse::InitGrid(bool start_center)
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

	if (start_center)
		RandomCollapseCenter();

	ready = true;
	shufflePatterns();
}

bool Bloom::WFC::WaveFunctionCollapse::isGridDone()
{
	// FIND LEAST ENTROPY TILE
	for (int y = 0; y < this->size.y; y++)
	{
		for (int x = 0; x < this->size.x; x++)
		{
			if (this->grid.at(y).at(x).superposition.size()> 1) {
				return false;
			}
		}
	}

	return true;
}

void Bloom::WFC::WaveFunctionCollapse::RandomCollapseCenter()
{
	auto least_entropy = this->size / 2.0f;

	int index = Utils::WeightedRandom(this->grid.at(least_entropy.y).at(least_entropy.x).superposition, this->weights);
	this->grid.at(least_entropy.y).at(least_entropy.x).superposition = std::vector<int>({ this->grid.at(least_entropy.y).at(least_entropy.x).superposition.at(index) });

	// PROPAGATE
	if (!(Propagate(least_entropy + glm::vec2(0, 1)) && Propagate(least_entropy + glm::vec2(0, -1)) && Propagate(least_entropy + glm::vec2(1, 0)) && Propagate(least_entropy + glm::vec2(-1, 0)))) {
		InitGrid();
	}
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

	this->patternHeight = pattern.size();
	this->patternWidth = pattern[0].size();

	this->pattern = pattern;
}

bool Bloom::WFC::Pattern::Match(const std::vector<std::vector<Bloom::WFC::Tile>>& target) const {
	const int targetWidth = target[0].size();
	const int targetHeight = target.size();
	const int offsetX = targetWidth / 2 - patternWidth / 2;
	const int offsetY = targetHeight / 2 - patternHeight / 2;

	for (int y = 0; y < patternHeight; y++) {
		const int targetY = y + offsetY;
		if (targetY < 0 || targetY >= targetHeight) {
			// Pattern exceeds the target boundary, no match possible
			return false;
		}

		for (int x = 0; x < patternWidth; x++) {
			const int targetX = x + offsetX;
			if (targetX < 0 || targetX >= targetWidth) {
				// Pattern exceeds the target boundary, no match possible
				return false;
			}

			const auto& currentTarget = target[targetY][targetX];
			if (std::find(currentTarget.superposition.begin(), currentTarget.superposition.end(), pattern[y][x]) == currentTarget.superposition.end()) {
				// Pattern element not found in the target superposition
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

int Bloom::WFC::Pattern::GetCenter() const
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

void Bloom::WFC::Tile::Print() const
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

	for (int i = pattern_size.y / 2; i < bitmap.size() - (pattern_size.y - pattern_size.y / 2); i++)
	{
		for (int j = pattern_size.x / 2; j < bitmap.at(i).size() - (pattern_size.x - pattern_size.x / 2); j++)
		{
			counter++;
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

			Pattern newPattern(pattern);

			// Check if the pattern already exists in the vector
			bool isDuplicate = false;
			for (const auto& existingPattern : patterns)
			{
				if (existingPattern.getPattern() == newPattern.getPattern())
				{
					isDuplicate = true;
					break;
				}
			}

			if (!isDuplicate)
			{
				patterns.push_back(newPattern);
			}
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
			bitmap[height-y-1][x] = static_cast<int>(pixelValue);
		}
	}

	stbi_image_free(image);

	return bitmap;
}

bool isElementInVector(const std::vector<int>& vec, int element) {
	for (const int& num : vec) {
		if (num == element) {
			return true;
		}
	}
	return false;
}

glm::vec2 Bloom::WFC::WaveFunctionCollapse::findBacktrackTile(const glm::vec2& currentTile) const {
	int startY = static_cast<int>(currentTile.y);
	int startX = static_cast<int>(currentTile.x);

	if (startY >= grid.size() || startX >= grid[startY].size()) {
		return glm::vec2(-1);
	}
	for (int y = startY; y >= 0; y--) {
		for (int x = (y == startY ? startX : grid[y].size() - 1); x >= 0; x--) {
			if (!grid[y][x].superposition.empty() && grid[y][x].superposition.size() > 1) {
				return glm::vec2(x, y);
			}
		}
	}

	return glm::vec2(-1);
}

bool Bloom::WFC::WaveFunctionCollapse::Backtrack(const glm::vec2& currentTile) {
	this->grid.at(currentTile.y).at(currentTile.x).superposition.clear(); // Reset the current tile

	// Continue backtracking to the previous tile
	glm::vec2 prevTile = findBacktrackTile(currentTile);
	if (prevTile != glm::vec2(-1)) {
		if (!this->grid.at(prevTile.y).at(prevTile.x).superposition.empty()) {
			int prevIndex = Utils::RandInt(0, this->grid.at(prevTile.y).at(prevTile.x).superposition.size() - 1);
			this->grid.at(prevTile.y).at(prevTile.x).superposition = std::vector<int>({ this->grid.at(prevTile.y).at(prevTile.x).superposition.at(prevIndex) });
			return Propagate(prevTile + glm::vec2(0, 1)) && Propagate(prevTile + glm::vec2(0, -1)) && Propagate(prevTile + glm::vec2(1, 0)) && Propagate(prevTile + glm::vec2(-1, 0));
		}
		else {
			// No more options available for backtracking, continue backtracking to the previous tile
			return Backtrack(prevTile);
		}
	}
	else {
		// No more tiles to backtrack, the algorithm has failed
		std::cout << "No more tiles to backtrack, the algorithm has failed\n";
		return false;
	}
}

bool Bloom::WFC::WaveFunctionCollapse::Collapse() {
	glm::vec2 least_entropy;

	// FIND LEAST ENTROPY
	least_entropy = leastEntropyTileIdx(this->grid);
	if (least_entropy == glm::vec2(-1)) {
		return true;
	}
	
	// COLLAPSE LEAST ENTROPY BY WEIGHT
	int index = Utils::WeightedRandom(this->grid.at(least_entropy.y).at(least_entropy.x).superposition, this->weights);
	this->grid.at(least_entropy.y).at(least_entropy.x).superposition = std::vector<int>({ this->grid.at(least_entropy.y).at(least_entropy.x).superposition.at(index) });
	
	// PROPAGATE
	if (!(Propagate(least_entropy + glm::vec2(0, 1)) && Propagate(least_entropy + glm::vec2(0, -1)) && Propagate(least_entropy + glm::vec2(1, 0)) && Propagate(least_entropy + glm::vec2(-1, 0)))) {
		// Backtrack
		glm::vec2 backtrackTile = findBacktrackTile(least_entropy);
		if (backtrackTile != glm::vec2(-1)) {
			auto res = Backtrack(backtrackTile);
			return res;
		}
		else {
			return false;
		}
	}

	//Print(this->grid);
	return true;
}

bool Bloom::WFC::WaveFunctionCollapse::Propagate(glm::vec2 idx)
{
	Pattern* pattern = nullptr;
	if (idx.y < this->size.y && idx.y >= 0 && idx.x < this->size.x && idx.x >= 0) {
		if (this->grid.at(idx.y).at(idx.x).GetEntropy() <= 1)
			return true;
		auto curridx = idx;
		bool matches = false;
		std::vector<int> newOptions;
		for (int i = 0; i < patterns.size(); i++)
		{
			pattern = &patterns.at(i);
			if (isElementInVector(newOptions, pattern->GetCenter()))
				continue;
			
			auto subset = GetSubset(this->grid, curridx, pattern->GetSize());
			
			if (pattern->Match(subset)) {
				newOptions.push_back(pattern->GetCenter());
				matches = true;
			}
		}
		
		this->grid.at(curridx.y).at(curridx.x).superposition = newOptions;
		
		if (!matches) {
			std::cout << "ERROR: No matches found for index (" << curridx.x << ", " << curridx.y << ")\n";
			return false;
		}
		return true;
	}

	return false;
}