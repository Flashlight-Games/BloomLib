#include "particles.h"

Bloom::Particle::Particle(int instance, float time)
{
	spawn_time = time;
	id = instance;
}

void Bloom::Particle::Update(float Gametime, float Gamespeed)
{
}

glm::mat4 Bloom::Particle::GetTransformMatrix()
{
	glm::mat4 model_matrix = glm::mat4(1.0);
	model_matrix = glm::translate(model_matrix, glm::vec3(this->location.x, this->location.y, 0.0f));
	model_matrix = glm::rotate(model_matrix, (float)glm::radians(this->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(this->scale.x, this->scale.y, 1.0f));
	return model_matrix;
}
