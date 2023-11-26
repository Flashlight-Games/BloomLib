#ifndef FB_BLOOM
#define FB_BLOOM

#include "framebuffers.h"

namespace Bloom {
	class Bloom_Framebuffer {
	public:
		Bloom_Framebuffer(glm::vec2 size);
		~Bloom_Framebuffer();

		void SetSize(float size);
		int GetSize() { return size; };

		void Use(bool clear);
		void Draw(bool include_objects = false, float depth = 0.99);
		void DrawToDefault(glm::vec2 window_size, bool include_objects = false, float depth = 0.99);
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