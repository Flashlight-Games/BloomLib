#ifndef GFLIP_AB
#define GFLIP_AB

#include "Ability.h"

namespace Application {
	namespace Abilities {
		class GravityFlip : public Ability {
		public:
			GravityFlip(Player* player);
			void Activate(float time);

			void Deactivate(float time);

			void Update(float time);
		private:
		};
	}
}

#endif // !GFLIP_AB