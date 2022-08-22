#ifndef BLINK_AB
#define BLINK_AB

#include "Ability.h"

namespace Application {
	namespace Abilities {
		class Blink : public Ability {
		public:
			Blink(Player* player);
			void Activate(float time);

			void Deactivate(float time);

			void Update(float time);
		private:
		};
	}
}

#endif // !TESTABILITY