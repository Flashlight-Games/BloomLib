#ifndef UTILS
#define UTILS

#include "dependencies.h"

namespace Utils {
    template <typename T> T Lerp(T A, T B, float percentage) {
        return A + (B - A) * percentage;
    };

    float Distance2D(glm::vec2 A, glm::vec2 B);

    glm::vec2 PointAtAngleAndDistance(float angle, float distance);

    void SyncSleep(int target_fps = 60);
    void SleepMS(int ms);

    int RandInt(int min, int max);

    float RandFloat(float min, float max);

    bool isDevelopmentConfiguration();

    void CopyFramebuffer(Bloom::Framebuffer* source, Bloom::Framebuffer* destination);

    float Noise_Simplex(glm::vec2 position, int seed = RandInt(-9999,9999));
    std::vector<float> Noise_Simplex(glm::vec2 position, glm::vec2 size,int seed = RandInt(-9999, 9999));
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
}

#endif