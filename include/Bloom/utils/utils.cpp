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

glm::vec2 Utils::PixelateResolution(glm::vec2 resolution, int new_height){
    return glm::vec2(resolution.x / (resolution.y / new_height), new_height);
}

void Utils::EnableWireframe()
{
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
}

void Utils::DisableWireframe()
{
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
}

std::string Utils::Black(bool background)
{
    return !background ? "\x1b[30m" : "\x1b[40m";
}
std::string Utils::Red(bool background)
{
    return !background ? "\x1b[31m" : "\x1b[41m";
}
std::string Utils::Green(bool background)
{
    return !background ? "\x1b[32m" : "\x1b[42m";
}
std::string Utils::Yellow(bool background)
{
    return !background ? "\x1b[33m" : "\x1b[43m";
}
std::string Utils::Blue(bool background)
{
    return !background ? "\x1b[34m" : "\x1b[44m";
}
std::string Utils::Magenta(bool background)
{
    return !background ? "\x1b[35m" : "\x1b[45m";
}
std::string Utils::Cyan(bool background)
{
    return !background ? "\x1b[36m" : "\x1b[46m";
}
std::string Utils::White(bool background)
{
    return !background ? "\x1b[37m" : "\x1b[47m";
}
std::string Utils::Default(bool background)
{
    return !background ? "\x1b[39m" : "\x1b[39m";
}

glm::vec2 Utils::rotateVector(glm::vec2 vector,  float degrees, glm::vec2 axis)
{
    float distance = Distance2D(axis, vector);
    float angle = atan2(axis.y-vector.y, axis.x - vector.x);
    return PointAtAngleAndDistance(angle + glm::radians(degrees), distance) + axis;
}

int Utils::RandInt(int min, int max) {
    return int(Lerp(min, (max+1), (rand() % (int)(max+1)) / (float)(max+1)));
}

float Utils::RandFloat(float min, float max, int precision) {
    float normalizedRand = static_cast<float>(rand()) / RAND_MAX;
    float scaledRand = min + normalizedRand * (max - min);
    return scaledRand;
}

int Utils::WeightedRandom(const std::vector<int>& options, const std::vector<int>& weights) {
    // Create a vector of valid weights based on the provided options
    std::vector<int> validWeights;
    for (int option : options) {
        validWeights.push_back(weights[option]);
    }

    // Calculate the total weight for the valid options
    int totalWeight = 0;
    for (int weight : validWeights) {
        totalWeight += weight;
    }

    // Generate a random number within the total weight range
    int randomNumber = Utils::RandInt(0, totalWeight - 1);

    // Find the index corresponding to the random number
    int currentIndex = 0;
    int index = 0;
    for (int option : options) {
        currentIndex += validWeights[index];
        if (randomNumber < currentIndex) {
            return index;
        }
        index++;
    }

    // Default case: return the index of the last option
    return options.size() - 1;
}

float Utils::Deadzone(float value, float range)
{
    if (abs(value) > range)
        return value;
    return 0.0;
}

glm::vec2 Utils::Deadzone(glm::vec2 value, glm::vec2 range)
{
    auto retval = glm::vec2(0);
    if (abs(value.x) > range.x)
        retval.x = value.x;
    if (abs(value.y) > range.y)
        retval.y = value.y;
    return retval;
}

glm::vec2 Utils::MovePointInDistance(const glm::vec2 point, const glm::vec2 center,const float distance)
{
    if (Utils::Distance2D(point, center) <= distance) {
        return point;
    }
    return glm::normalize(point - center) * distance + center;
}

bool Utils::CreateDirectories(std::string path) {
    unsigned int pos = 0;
    do
    {
        pos = path.find_first_of("\\/", pos + 1);


        wchar_t wtext[128];
        mbstowcs(wtext, path.substr(0, pos).c_str(), path.substr(0, pos).length() + 1);//Plus null

        if (!(CreateDirectory(wtext, NULL) ||
            ERROR_ALREADY_EXISTS == GetLastError())) {
            return false;
        }
    } while (pos != std::string::npos);
    return true;
}

bool Utils::DeleteDirectory(const std::string& path) {
    std::string command = "rmdir /s /q \"" + path + "\" 2>NUL";
    int result = std::system(command.c_str());
    return (result == 0 || result == 2);
}

void Utils::printBits(unsigned int value) {
    int numBits = sizeof(value) * 8; // Assuming 8 bits per byte

    for (int i = numBits - 1; i >= 0; i--) {
        bool bit = (value & (1u << i)) != 0;
        std::cout << bit;
    }
    std::cout << std::endl;
}

