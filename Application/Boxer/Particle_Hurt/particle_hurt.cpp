#include "particle_hurt.h"

Bloom::Particle_HURT::Particle_HURT(GLFWwindow* window, int instance, float time, glm::vec2 initial_location) : Bloom::Particle::Particle(window,instance,time, initial_location) {
	SetLocation(initial_location);
	SetScale(glm::vec2(0.1));
	SetLifetime(0.5);
}

void Bloom::Particle_HURT::Update(float Gametime, float Gamespeed)
{
	SetLocation(GetLocation() + this->initial_velocity * glm::vec2(Gamespeed));
	SetScale(glm::vec2(0.5 * (1 - (GetAge() / GetLifetime()))));
}