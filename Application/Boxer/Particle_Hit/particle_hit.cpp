#include "particle_hit.h"

Bloom::Particle_HIT::Particle_HIT(GLFWwindow* window, int instance, float time, glm::vec2 initial_location) : Bloom::Particle::Particle(window,instance,time, initial_location) {
	SetLocation(initial_location);
	SetScale(glm::vec2(0.1));
	SetLifetime(0.5);
	SetColor(glm::vec4(1, 0.25, 0, 1));
}

void Bloom::Particle_HIT::Update(float Gametime, float Gamespeed)
{
	SetLocation(GetLocation() + this->initial_velocity * glm::vec2(Gamespeed));
	SetScale(glm::vec2(0.25 * (1 - (GetAge() / GetLifetime()))));
}