#ifndef DASH_AB
#define DASH_AB

#include "Ability.h"

namespace Application {
	namespace Abilities {
		class Dash : public Ability {
		public:
			Dash(Player* player);
			void Activate(float time);

			void Deactivate(float time);

			void Update(float time);
		private:
		};
	}
}

#endif // !TESTABILITY