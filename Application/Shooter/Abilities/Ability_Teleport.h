#ifndef TELEPORT_AB
#define TELEPORT_AB

#include "Ability.h"

namespace Application {
	namespace Abilities {
		class Teleport : public Ability {
		public:
			Teleport(Player* player);
			void Activate(float time);

			void Deactivate(float time);

			void Update(float time);
		private:
		};
	}
}

#endif // !TESTABILITY