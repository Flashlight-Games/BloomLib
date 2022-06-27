#include "bloom.h"

Bloom::Bloom_Framebuffer::Bloom_Framebuffer(int width, int height){
	bloom_f = new Framebuffer(width, height, true);
	bloom_f->setShader("res/engine/shaders/framebuffer/framebuffer_bloom_filter.glsl");

	bloom_h = new Framebuffer(width,height,true);
	bloom_h->setShader("res/engine/shaders/framebuffer/framebuffer_bloom_horizontal.glsl");

	bloom_v = new Framebuffer(width, height, true);
	bloom_v->setShader("res/engine/shaders/framebuffer/framebuffer_bloom_vertical.glsl");
}

void Bloom::Bloom_Framebuffer::SetSize(float size) {
	this->size = size;
}

void Bloom::Bloom_Framebuffer::Draw() { // TWO PASS BLOOM

	bloom_h->SetTexture(bloom_f->getTexture());
	bloom_v->use(true);
	bloom_h->draw();
	bloom_v->drawToDefault(glm::vec2(900,900));
};

void Bloom::Bloom_Framebuffer::DrawToDefault(glm::vec2 window_size) {
	bindDefaultFramebuffer(true, window_size.x, window_size.y); Draw();
}

void Bloom::Bloom_Framebuffer::Use(bool clear) {
	bloom_f->use(clear);
}