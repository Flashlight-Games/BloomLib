#include "chromatic_aberration.h"

Bloom::Chromatic_Aberration::Chromatic_Aberration(int width, int height) : Framebuffer::Framebuffer(width, height, true, false) {
	setShader("res/engine/shaders/framebuffer/framebuffer_chromatic_aberration.glsl");
	GetShader().use();
	size_uid = GetShader().getUniformId("uSize");
}

void Bloom::Chromatic_Aberration::SetSize(float size) {
	this->size = size;
	GetShader().use();
	GetShader().uniformFloat(size_uid, this->size);
}