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
		void HTextureGUI() { return bloom_h->getTexture()->ImGui(); };
		void VTextureGUI() { return bloom_v->getTexture()->ImGui(); };
	private:
		Framebuffer* bloom_o;
		Framebuffer* bloom_f;
		Framebuffer* bloom_h;
		Framebuffer* bloom_v;
		int size = 10;

		int intensity_uid;

		int mipLevels = -1;
		std::vector<Bloom::Texture2D*> mips;
	};
}

#endif