#include "utils.h"

float Utils::Distance2D(glm::vec2 A, glm::vec2 B)
{
    return std::sqrt(std::pow(A.x - B.x, 2) + std::pow(A.y - B.y, 2));
}

glm::vec2 Utils::PointAtAngleAndDistance(float angle, float distance) {
    return glm::vec2(distance * cos(angle), (distance * sin(angle)));
}

void Utils::SyncSleep(int target_fps) {
    std::this_thread::sleep_for(std::chrono::milliseconds((int)((1000.0 / target_fps))));
}

void Utils::SleepMS(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Utils::CopyFramebuffer(Bloom::Framebuffer* source, Bloom::Framebuffer* destination)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, source->getFBO());
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, source->getTexture()->GetID(), 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination->getFBO());
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, destination->getTexture()->GetID(), 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glBlitFramebuffer(
        0, 0, source->getWidth(), source->getHeight(),
        0, 0, destination->getWidth(), destination->getHeight(),
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
}

int Utils::RandInt(int min, int max) {
    return int(Lerp(min, max, (rand() % 10) / 10.0));
}

float Utils::RandFloat(float min, float max) {
    return Lerp(min, max, (rand() % 10) / 10.0);
}

bool Utils::isDevelopmentConfiguration() { return *ENGINE_CONFIGURATION == *"Development"; }