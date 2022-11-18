#ifndef CHROMATIC_ABERRATION
#define CHROMATIC_ABERRATION

#include "framebuffers.h"

namespace Bloom {
	class Chromatic_Aberration_Framebuffer : public Framebuffer {
	public:
		Chromatic_Aberration_Framebuffer(glm::vec2 size);
		void SetSize(float size);
		float GetSize() { return size; };
	private:
		float size = 0.01;
		int size_uid = 0;
	};
}

#endif