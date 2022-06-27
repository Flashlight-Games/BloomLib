#ifndef FB_BLOOM
#define FB_BLOOM

#include "framebuffers.h"

namespace Bloom {
	class Bloom_Framebuffer {
	public:
		Bloom_Framebuffer(int width, int height);
		void SetSize(float size);
		int GetSize() { return size; };

		void Use(bool clear);
		void Draw();
		void DrawToDefault(glm::vec2 window_size);
	private:
		Framebuffer* bloom_f;
		Framebuffer* bloom_h;
		Framebuffer* bloom_v;
		int size = 0;
	};
}

#endif