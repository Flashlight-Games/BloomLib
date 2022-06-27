#include "bloom.h"

Bloom::Bloom_Framebuffer::Bloom_Framebuffer(int width, int height){
	bloom_f = new Framebuffer(width, height, true);
	bloom_f->setShader("res/engine/shaders/framebuffer/framebuffer_bloom_filter.glsl");

	bloom_h = new Framebuffer(width,height,true);
	bloom_h->setShader("res/engine/shaders/framebuffer/framebuffer_bloom_horizontal.glsl");
	
	bloom_v = new Framebuffer(width, height, true);
	bloom_v->setShader("res/engine/shaders/framebuffer/framebuffer_bloom_vertical.glsl");
	
	bloom_v->GetShader().use();
	v_size_uid = bloom_v->GetShader().getUniformId("uSize");

	bloom_h->GetShader().use();
	h_size_uid = bloom_h->GetShader().getUniformId("uSize");
}

void Bloom::Bloom_Framebuffer::SetSize(float size) {
	this->size = size;
}

void Bloom::Bloom_Framebuffer::Draw() { // TWO PASS 
	bloom_h->GetShader().use();
	bloom_h->GetShader().uniformInt(h_size_uid, size);
	bloom_v->GetShader().use();
	bloom_v->GetShader().uniformInt(v_size_uid, this->size);

	bloom_h->SetTexture(bloom_f->getTexture());
	bloom_v->use(true);
	bloom_h->draw();

	bloom_v->draw();
	bloom_f->draw();
};

void Bloom::Bloom_Framebuffer::DrawToDefault(glm::vec2 window_size) {
	bloom_h->GetShader().use();
	bloom_h->GetShader().uniformInt(h_size_uid, size);
	bloom_v->GetShader().use();
	bloom_v->GetShader().uniformInt(v_size_uid, this->size);

	bloom_h->use(true);
	bloom_f->draw();
	bloom_v->use(true);
	bloom_h->draw();

	
	
	bloom_v->drawToDefault(window_size,false);
	bloom_f->draw();
	
}

void Bloom::Bloom_Framebuffer::Use(bool clear) {
	bloom_f->use(clear);
}