#include "particle_death.h"

Bloom::Particle_DEATH::Particle_DEATH(int instance, float time) : Bloom::Particle::Particle(instance,time) {
	SetScale(glm::vec2(0.1));
	SetLifetime(0.25);
	SetColor(glm::vec4(1, 0.25, 0, 1));

	SetInitialVelocity(glm::vec2((((rand() % 10 / 10.0)+0.5) * 2 - 1) * 50, (((rand() % 10 / 10.0) + 0.5) * 2 - 1) * 50));
}

void Bloom::Particle_DEATH::Update(float Gametime, float Gamespeed)
{
	SetLocation(GetLocation() + this->initial_velocity * glm::vec2(Gamespeed));
	SetScale(glm::vec2(0.25 * (1 - (GetAge() / GetLifetime()))));
}