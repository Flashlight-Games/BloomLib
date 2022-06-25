#ifndef CHROMATIC_ABERRATION
#define CHROMATIC_ABERRATION

#include "framebuffers.h"

namespace Bloom {
	class Chromatic_Aberration : public Framebuffer {
	public:
		Chromatic_Aberration(int width, int height);
		void SetSize(float size);
		float GetSize() { return size; };
	private:
		float size = 0.01;
		int size_uid = 0;
	};
}

#endif