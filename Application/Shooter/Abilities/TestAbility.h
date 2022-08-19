#ifndef TESTABILITY
#define TESTABILITY

#include "Ability.h"

namespace Application {
	namespace Abilities {
		class Test : public Ability {
		public:
			Test(Player* player) : Ability(player, 2.0) {};
			void Activate(float time) {
				Ability::Activate(time);
			}

			void Deactivate(float time) {
				Ability::Deactivate(time);
			}

			void Update(float time) {
				Ability::Update(time);
				if (GetActive()) {
					if (this->GetActiveDuration() < 1.0)
						GetPlayer()->GetKinematics()->ApplyForce(glm::vec2(0, 9.8));
					else
						Deactivate(time);
				}
			}
		private:
		};
	}
}

#endif // !TESTABILITY