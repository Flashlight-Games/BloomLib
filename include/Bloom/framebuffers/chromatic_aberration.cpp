#include "chromatic_aberration.h"

Bloom::Chromatic_Aberration_Framebuffer::Chromatic_Aberration_Framebuffer(glm::vec2 size) : Framebuffer::Framebuffer(size, true, false) {
	setShader("include/Bloom/res/engine/shaders/framebuffer/framebuffer_chromatic_aberration.glsl");
	GetShader().use();
	size_uid = GetShader().getUniformId("uSize");
}

void Bloom::Chromatic_Aberration_Framebuffer::SetSize(float size) {
	this->size = size;
	GetShader().use();
	GetShader().uniformFloat(size_uid, this->size);
}