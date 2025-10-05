#include <random>
#include <GLM/glm.hpp>
#include <GLFW/glfw3.h>
#include <FastNoise/FastNoise.h>
#define NOMINMAX
#include <windows.h>

export module Bloom.Utils;

import std;

std::random_device rd;
std::mt19937 eng(rd());
std::uniform_real_distribution<> distr(0.0, 1.0); // Distribution in range [0, 1)

export namespace Utils {
    template <typename T>
    T constexpr Lerp(T A, T B, float percentage) {
        return A + (B - A) * percentage;
    }

    template <typename T>
    T constexpr Lerp_Continuous(T A, T B, float percentage) {
        if (B - A > 180) A += 360;
        else if (A - B > 180) A -= 360;
        return A + (B - A) * percentage;
    }

    template <typename T>
    T constexpr Corp(T A, T B, T speed){
        T delta = B - A;
        if (delta < speed)
            return B;
        return A + delta;
    }

    glm::vec2 Corp(glm::vec2 A, glm::vec2 B, float speed) {
        glm::vec2 delta = B - A;
        if (glm::length(delta) < speed)
            return B;
        return A + glm::normalize(delta) * speed;
    }

    glm::vec2 PointAtAngleAndDistance(float angle_radians, float distance) {
        return glm::vec2(distance * cos(angle_radians), (distance * sin(angle_radians)));
    }

    int RandInt(int min, int max) {
        return glm::round(Lerp(min, max+1, distr(eng)));
    }

    float RandFloat(float min, float max, int precision = 1000) {
        float scaledRand = min + distr(eng) * (max - min);
        return scaledRand;
    }

    template<typename NoiseType>
    float NoiseWithFrequency(glm::vec2 position, int frequency = 1, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<NoiseType>();
        std::vector<float> output(3 * 3);
        noise->GenUniformGrid2D(output.data(), position.x, position.y, 3, 3, frequency, seed);
        return output.at(0);
    }

    template<typename NoiseType>
    float Noise(glm::vec2 position, int seed = RandInt(-9999, 9999)) {
        return NoiseWithFrequency<NoiseType>(position, 1, seed);
    }

    template<typename NoiseType>
    std::vector<float> Noise(glm::vec2 position, glm::vec2 size, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<NoiseType>();
        std::vector<float> output(size.x * size.y);
        noise->GenUniformGrid2D(output.data(), position.x, position.y, size.x, size.y, 1, seed);
        return output;
    }

    float Noise_White(glm::vec2 position, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::White>();
        return noise->GenSingle2D(position.x, position.y, seed);
    }

    float Noise_Uniform(glm::vec2 position, unsigned int seed = RandInt(0, 9999)) {
        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
        unsigned int combined_seed = seed ^ ((int)position.x + (int)position.y * 0x9E3779B9);
        generator.seed(combined_seed);

        float value = distribution(generator);
        return value * 2.0f - 1.0f;
    }

    float Noise_Uniform(float position, unsigned int seed = RandInt(0, 9999)) {
        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
        unsigned int combined_seed = seed ^ (static_cast<int>(position) * 0x9E3779B9);
        generator.seed(combined_seed);

        float value = distribution(generator);
        return value * 2.0f - 1.0f;
    }

    float Noise_Simplex(glm::vec2 position, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Simplex>();
        return noise->GenSingle2D(position.x, position.y, seed);
    }

    std::vector<float> Noise_Simplex(glm::vec2 position, glm::vec2 size, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Simplex>();
        std::vector<float> output(size.x * size.y);
        noise->GenUniformGrid2D(output.data(), position.x, position.y, size.x, size.y, 1, seed);
        return output;
    }

    float Noise_Value(glm::vec2 position, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Value>();
        return noise->GenSingle2D(position.x, position.y, seed);
    }

    std::vector<float> Noise_Value(glm::vec2 position, glm::vec2 size, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Value>();
        std::vector<float> output(size.x * size.y);
        noise->GenUniformGrid2D(output.data(), position.x, position.y, size.x, size.y, 1, seed);
        return output;
    }

    float map(float input, float input_start, float input_end, float output_start, float output_end) {
        double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
        return (output_start + slope * (input - input_start));
    }

    glm::vec3 HSVtoRGB(glm::vec3 hsv) {
        auto H = glm::clamp(hsv.r, 0.f, 360.f);
        auto S = glm::clamp(hsv.g, 0.f, 100.f);
        auto V = glm::clamp(hsv.b, 0.f, 100.f);

        float s = S / 100;
        float v = V / 100;
        float C = s * v;
        float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
        float m = v - C;
        float r, g, b;
        if (H >= 0 && H < 60) {
            r = C, g = X, b = 0;
        }
        else if (H >= 60 && H < 120) {
            r = X, g = C, b = 0;
        }
        else if (H >= 120 && H < 180) {
            r = 0, g = C, b = X;
        }
        else if (H >= 180 && H < 240) {
            r = 0, g = X, b = C;
        }
        else if (H >= 240 && H < 300) {
            r = X, g = 0, b = C;
        }
        else {
            r = C, g = 0, b = X;
        }
        float R = (r + m) * 1;
        float G = (g + m) * 1;
        float B = (b + m) * 1;
        return glm::vec3(R, G, B);
    }

    glm::vec2 PixelateResolution(glm::vec2 resolution, int new_height) {
        return glm::vec2(resolution.x / (resolution.y / new_height), new_height);
    }

    std::string Black(bool background = false) {
        return !background ? "\x1b[30m" : "\x1b[40m";
    }
    std::string Red(bool background = false) {
        return !background ? "\x1b[31m" : "\x1b[41m";
    }
    std::string Green(bool background = false) {
        return !background ? "\x1b[32m" : "\x1b[42m";
    }
    std::string Yellow(bool background = false) {
        return !background ? "\x1b[33m" : "\x1b[43m";
    }
    std::string Blue(bool background = false) {
        return !background ? "\x1b[34m" : "\x1b[44m";
    }
    std::string Magenta(bool background = false) {
        return !background ? "\x1b[35m" : "\x1b[45m";
    }
    std::string Cyan(bool background = false) {
        return !background ? "\x1b[36m" : "\x1b[46m";
    }
    std::string White(bool background = false) {
        return !background ? "\x1b[37m" : "\x1b[47m";
    }
    std::string Default(bool background = false) {
        return !background ? "\x1b[39m" : "\x1b[49m";
    }

    void Error(std::string message) {
        throw std::runtime_error(Red(false) + message + Default(false));
    }

    glm::vec2 rotateVector(glm::vec2 vector, float degrees, glm::vec2 axis = glm::vec2(0)) {
        float distance = glm::distance(axis, vector);
        float angle = atan2(axis.y - vector.y, axis.x - vector.x);
        return PointAtAngleAndDistance(angle + glm::radians(degrees), distance) + axis;
    }

    int WeightedRandom(const std::vector<int>& options, const std::vector<int>& weights) {
        std::vector<int> validWeights;
        for (int option : options) {
            validWeights.push_back(weights[option]);
        }

        int totalWeight = 0;
        for (int weight : validWeights) {
            totalWeight += weight;
        }

        int randomNumber = Utils::RandInt(0, totalWeight - 1);

        int currentIndex = 0;
        int index = 0;
        for (int option : options) {
            currentIndex += validWeights[index];
            if (randomNumber < currentIndex) {
                return index;
            }
            index++;
        }

        return options.size() - 1;
    }

    template<typename T>
    T WeightedRandom(const std::map<T, float>& options) {
        std::vector<T> validKeys;
        std::vector<float> validWeights;
        float totalWeight = 0;

        for (const auto& option : options) {
            validKeys.push_back(option.first);
            validWeights.push_back(option.second);
            totalWeight += option.second;
        }

        float randomValue = Utils::RandFloat(0, totalWeight, 100);

        float currentValue = 0;
        for (size_t index = 0; index < validWeights.size(); ++index) {
            currentValue += validWeights[index];
            if (randomValue < currentValue) {
                return validKeys[index];
            }
        }

        return validKeys.back();
    }

    glm::vec2 MovePointInDistance(glm::vec2 point, glm::vec2 center, float distance) {
        if (glm::distance(point, center) <= distance) {
            return point;
        }
        return glm::normalize(point - center) * distance + center;
    }

    void printBits(unsigned int value) {
        int numBits = sizeof(value) * 8; // Assuming 8 bits per byte

        for (int i = numBits - 1; i >= 0; i--) {
            bool bit = (value & (1u << i)) != 0;
            std::cout << bit;
        }
        std::cout << std::endl;
    }

    bool CopyToClipboard(std::string text) {
        if (text != "") {
            if (!OpenClipboard(NULL)) {
                return false;
            }
            EmptyClipboard();

            const char* textToCopy = text.c_str();
            size_t textSize = strlen(textToCopy) + 1;
            HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, textSize);
            if (hText == NULL) {
                CloseClipboard();
                return false;
            }

            LPVOID pText = GlobalLock(hText);
            memcpy(pText, textToCopy, textSize);

            GlobalUnlock(hText);
            SetClipboardData(CF_TEXT, hText);
            CloseClipboard();

            return true;
        }
        return true;
    }
    std::string GetClipboard() {
        if (!OpenClipboard(NULL)) {
            return "";
        }

        HANDLE hClipboardData = GetClipboardData(CF_TEXT);
        if (hClipboardData == NULL) {
            std::cerr << "Error getting clipboard data" << std::endl;
            CloseClipboard();
            return "";
        }

        LPSTR pClipboardText = static_cast<LPSTR>(GlobalLock(hClipboardData));
        if (pClipboardText == NULL) {
            std::cerr << "Error locking clipboard memory" << std::endl;
            CloseClipboard();
            return "";
        }
        std::string clipboardString(pClipboardText);

        GlobalUnlock(hClipboardData);
        CloseClipboard();

        return clipboardString;
    }

    int GCD(int a, int b) { // greatest common denominator
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
    }

    template<typename T>
    struct lazy {
        template<typename... Args>
        lazy(Args&&... args) : init([args...] { return T(args...); }) {}

        operator const T& () const {
            static T instance = init();
            return instance;
        }

        private:
            std::function<T()> init;
    };

    bool isPowerOfTwo(int x) {
        return x > 0 && (x & (x - 1)) == 0;
    }
}