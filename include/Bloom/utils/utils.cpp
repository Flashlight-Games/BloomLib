#include "utils.h"

float Utils::Distance2D(glm::vec2 A, glm::vec2 B)
{
    return std::sqrt(std::pow(A.x - B.x, 2) + std::pow(A.y - B.y, 2));
}

glm::vec2 Utils::PointAtAngleAndDistance(float angle_radians, float distance) {
    // RADIANS!
    return glm::vec2(distance * cos(angle_radians), (distance * sin(angle_radians)));
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

float Utils::Noise_Simplex(glm::vec2 position,int seed)
{
    FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Simplex>();
    std::vector<float> output(3*3);
    noise->GenUniformGrid2D(output.data(), position.x, position.y, 3, 3, 1, seed);
    return output.at(0);
}

std::vector<float> Utils::Noise_Simplex(glm::vec2 position, glm::vec2 size, int seed)
{
    FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Simplex>();
    std::vector<float> output(size.x * size.y);
    noise->GenUniformGrid2D(output.data(), position.x, position.y, size.x, size.y, 1, seed);
    return output;
}

float Utils::Noise_Value(glm::vec2 position, int seed)
{
    FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Value>();
    std::vector<float> output(5 * 5);
    noise->GenUniformGrid2D(output.data(), position.x, position.y, 5, 5, 1, seed);
    return output.at(0);
}

std::vector<float> Utils::Noise_Value(glm::vec2 position, glm::vec2 size, int seed)
{
    FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::Value>();
    std::vector<float> output(size.x * size.y);
    noise->GenUniformGrid2D(output.data(), position.x, position.y, size.x, size.y, 1, seed);
    return output;
}

bool Utils::isKeyPressed(GLFWwindow* window, int key)
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}
bool Utils::isKeyReleased(GLFWwindow* window, int key)
{
    return glfwGetKey(window, key) == GLFW_RELEASE;
}
bool Utils::isKeyHeld(GLFWwindow* window, int key)
{
    return glfwGetKey(window, key) == GLFW_REPEAT;
}

float Utils::map(float input, float input_start, float input_end, float output_start, float output_end)
{
    double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
    return (output_start + slope * (input - input_start));
}

float Utils::clamp(float input, float min, float max)
{
    return std::min(std::max(input,min),max);
}

glm::vec3 Utils::HSVtoRGB(glm::vec3 hsv)
{
    auto H = Utils::clamp(hsv.r,0,360);
    auto S = Utils::clamp(hsv.g,0,100);
    auto V = Utils::clamp(hsv.b,0,100);

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

glm::vec2 Utils::PixelateResolution(glm::vec2 resolution, int pixels_per_pixel){
    return resolution / glm::vec2(pixels_per_pixel);
}

int Utils::RandInt(int min, int max) {
    return int(Lerp(min, max, (rand() % (int)max) / (float)max));
}

float Utils::RandFloat(float min, float max) {
    return Lerp(min, max, (rand() % (int)ceil((max != 0) ? max : 100)) / (float)ceil((max!=0) ? max : 100));
}

bool Utils::isDevelopmentConfiguration() { return *ENGINE_CONFIGURATION == *"Development"; }