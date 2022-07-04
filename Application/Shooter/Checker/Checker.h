#ifndef CHECKER
#define CHECKER

#include "particles/particles.h"
#include "utils/utils.h"

namespace Application {
	class CheckParticles : public Bloom::Particle {
	public:
		CheckParticles(int instance, float time) : Bloom::Particle(instance,time) {
			SetLifetime(1.5);
		}
		void Update(float Gametime, float Gamespeed) {
			SetColor(glm::vec4(this->GetColor().r, this->GetColor().g, this->GetColor().b, 1 - (GetAge() / GetLifetime())));
		}
	private:

	};

	class Checker { 
	public:
		Checker(int amount, glm::mat4 view_matrix,glm::mat4 projection_matrix) {
			this->amount = amount;
			this->emmiter = new Bloom::ParticleEmitter<CheckParticles>(this->amount, view_matrix, projection_matrix);
		}

		void SetPosition(glm::vec2 position,GLFWwindow* window,float time,PhysX::collisionBox* other) {
			posA = posB;
			if (posB == glm::vec2(-1, -1))
				posA = position;
			posB = position;

			auto particles = emmiter->Play(window,time);
			bool hit = PhysX::LineTrace("main", other, GetStart(), GetEnd());
			for (int i = 0; i < particles.size(); i++)
			{
				particles.at(i)->SetLocation(glm::vec2(Utils::Lerp(posA, posB, (i+1) / (float)amount).x, Utils::Lerp(posA, posB, (i + 1) / (float)amount).y));
				//particles.at(i)->SetScale(glm::vec2(Utils::Distance2D(posA, posB) / 16.0 / amount));
				particles.at(i)->SetColor(hit ? glm::vec4(1, 0, 0, 1) : glm::vec4(1, 1, 1, 1));
			}
		}
		void Draw(float gametime, float gamespeed) { emmiter->Draw(gametime,gamespeed); };

		glm::vec2 GetStart() { return posA; };
		glm::vec2 GetEnd() { return posB; };
	private:
		Bloom::ParticleEmitter<Application::CheckParticles>* emmiter = nullptr;

		int amount;
		bool setA = false;
		glm::vec2 posA = glm::vec2(-1, -1);
		glm::vec2 posB = glm::vec2(-1, -1);
	};
}
#endif