#ifndef UTILS
#define UTILS

#include "Bloom/dependencies.h"
#include <cstdio>
#include <queue>

namespace Utils {
    template <typename T> T Lerp(T A, T B, float percentage) {
        return A + (B - A) * percentage;
    };

    template <typename T> T Corp(T A, T B, T speed) {
        T delta = B - A;
        if (delta < speed)
            return B;
        return A + delta;
    };

    glm::vec2 Corp(glm::vec2 A, glm::vec2 B, float speed);

    float Distance2D(glm::vec2 A, glm::vec2 B);

    glm::vec2 PointAtAngleAndDistance(float angle_radians, float distance);

    void SyncSleep(int target_fps = 60);
    void SleepMS(int ms);

    int RandInt(int min, int max);

    float RandFloat(float min, float max, int precision = 1000);

    void CopyFramebuffer(Bloom::Framebuffer* source, Bloom::Framebuffer* destination);
    
    template<typename NoiseType>
    float Noise(glm::vec2 position, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<NoiseType>();
        std::vector<float> output(3 * 3);
        noise->GenUniformGrid2D(output.data(), position.x, position.y, 3, 3, 1, seed);
        return output.at(0);
    };

    template<typename NoiseType>
    float NoiseWithFrequency(glm::vec2 position,int frequency = 1, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<NoiseType>();
        std::vector<float> output(3 * 3);
        noise->GenUniformGrid2D(output.data(), position.x, position.y, 3, 3, frequency, seed);
        return output.at(0);
    };

    template<typename NoiseType>
    std::vector<float> Noise(glm::vec2 position, glm::vec2 size, int seed = RandInt(-9999, 9999)) {
        FastNoise::SmartNode<> noise = FastNoise::New<NoiseType>();
        std::vector<float> output(size.x * size.y);
        noise->GenUniformGrid2D(output.data(), position.x, position.y, size.x, size.y, 1, seed);
        return output;
    };
    
    float Noise_White(glm::vec2 position, int seed = RandInt(-9999, 9999));

    float Noise_Simplex(glm::vec2 position, int seed = RandInt(-9999, 9999));
    std::vector<float> Noise_Simplex(glm::vec2 position, glm::vec2 size, int seed = RandInt(-9999, 9999));
    float Noise_Value(glm::vec2 position, int seed = RandInt(-9999, 9999));
    std::vector<float> Noise_Value(glm::vec2 position, glm::vec2 size, int seed = RandInt(-9999, 9999));

    template <typename T> T* GetApp(GLFWwindow* window) {
        T* app = static_cast<T*>(glfwGetWindowUserPointer(window));
        return app;
    };

    bool isKeyPressed(GLFWwindow* window, int key);
    bool isKeyReleased(GLFWwindow* window, int key);
    bool isKeyHeld(GLFWwindow* window, int key);

    float map(float input, float input_start, float input_end, float output_start, float output_end);
    float clamp(float input, float min, float max);

    glm::vec3 HSVtoRGB(glm::vec3 hsv);

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    glm::vec2 PixelateResolution(glm::vec2 resolution, int pixels_per_pixel);

    void EnableWireframe();
    void DisableWireframe();

    std::string Black(bool background = false);
    std::string Red(bool background = false);
    std::string Green(bool background = false);
    std::string Yellow(bool background = false);
    std::string Blue(bool background = false);
    std::string Magenta(bool background = false);
    std::string Cyan(bool background = false);
    std::string White(bool background = false);
    std::string Default(bool background = false);

    void Error(std::string message);

    glm::vec2 rotateVector(glm::vec2 vector, float degrees, glm::vec2 axis = glm::vec2(0));

    int WeightedRandom(const std::vector<int>& options, const std::vector<int>& weights);

    float Deadzone(float value, float range);

    glm::vec2 Deadzone(glm::vec2 value, glm::vec2 range);

    glm::vec2 MovePointInDistance(glm::vec2 point, glm::vec2 center, float distance);

    bool CreateDirectories(std::string path);

    bool DeleteDirectory(const std::string& path);

    void printBits(unsigned int value);

    bool CopyToClipboard(std::string text);
    std::string GetClipboard();

    template <typename T>
    bool queueContains(std::queue<T> queue, const T& value) {
        while (!queue.empty()) {
            if (queue.front() == value) {
                return true;
            }
            queue.pop();
        }
        return false;
    };

    template <typename T> T FindMostFrequent(std::vector<T> numbers, T exclude, T default_value);
    template<typename T>
    T FindMostFrequent(std::vector<T> numbers, T exclude, T default_value)
    {
        std::unordered_map<T, int> count;

        for (int i = 0; i < numbers.size(); i++) {
            if (numbers[i] != exclude) {
                count[numbers[i]]++;
            }
        }

        T mostFrequentNumber = default_value;
        int maxFrequency = 0;
        bool isAmbiguous = false;

        for (const auto& pair : count) {
            if (pair.second > maxFrequency) {
                mostFrequentNumber = pair.first;
                maxFrequency = pair.second;
                isAmbiguous = false;
            }
            else if (pair.second == maxFrequency && pair.first != exclude) {
                isAmbiguous = true;
            }
        }

        if (isAmbiguous) {
            return numbers.at(0);  // Ambiguous most frequent numbers
        }
        else if (mostFrequentNumber == default_value) {
            return default_value; // No valid options
        }
        else {
            return mostFrequentNumber;
        }
    }

    template<typename T>
    T GetBezierAtPercentage(std::vector<T> points, float percentage) {
        int edges = points.size() - 1;
        std::vector<T> locations;
        for (int i = 0; i < edges; i++)
        {
            T curr = points.at(i);
            T next = points.at(i + 1);

            T interp_location = Utils::Lerp(curr, next, percentage);

            if(edges == 1)
                return interp_location;
            else if (edges > 1) 
                locations.push_back(interp_location);
        }
        if (edges > 1)
            return GetBezierAtPercentage(locations, percentage);
    }

    double sinWithPhaseShift(double x, double phaseShiftDegrees = 0);

    double cosWithPhaseShift(double x, double phaseShiftDegrees = 0);

    glm::vec2 CalculateWalkCycleOffset(float time, float max_move_speed, glm::vec2 step_size, float phase_shift_degrees = 0, glm::vec2 direction = glm::vec2(0), bool inverted = false);

    float roundToNearest(float value, float interval);
}
#endif