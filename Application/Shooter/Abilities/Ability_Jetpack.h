#ifndef JETPACK_AB
#define JETPACK_AB

#include "Ability.h"

namespace Application {
	namespace Abilities {
		class Jetpack : public Ability {
		public:
			Jetpack(Player* player);
			void Activate(float time);

			void Deactivate(float time);

			void Update(float time);
		private:
		};
	}
}

#endif // !JETPACK