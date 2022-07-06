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

    float Noise2D_Simplex(glm::vec2 size, int seed = RandInt(-9999,9999));
}

#endif