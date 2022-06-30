#include "bloom.h"

Bloom::Bloom_Framebuffer::Bloom_Framebuffer(int width, int height){
	bloom_o = new Framebuffer(width, height, true,true);
	bloom_o->setShader("res/engine/shaders/framebuffer/framebuffer_fragment_clear.glsl");

	bloom_f = new Framebuffer(width, height, true, true);
	bloom_f->setShader("res/engine/shaders/framebuffer/framebuffer_fragment_filter.glsl");

	bloom_h = new Framebuffer(width,height,true, true); // SHRINKER
	bloom_h->setShader("res/engine/shaders/framebuffer/framebuffer_downsample.glsl");
	
	bloom_v = new Framebuffer(width, height, true, true);
	bloom_v->setShader("res/engine/shaders/framebuffer/framebuffer_fragment.glsl");
	
	bloom_f->GetShader().use();
	intensity_uid= bloom_f->GetShader().getUniformId("intensity");

	int lastSizeX = bloom_f->getWidth();
	int lastSizeY = bloom_f->getHeight();
	int counter = 1;
	while (counter < 6) {
		counter++;
		this->mipLevels++;
		this->mips.push_back(new Bloom::Texture2D(nullptr, glm::vec2(lastSizeX / 2, lastSizeY / 2)));
		lastSizeX /= 2;
		lastSizeY /= 2;
		std::cout << counter << "\n";
	}
	
}

void Bloom::Bloom_Framebuffer::SetSize(float size) {
	this->size = size * 10;
}

void Bloom::Bloom_Framebuffer::Draw( bool include_objects) { // TWO PASS 
	bloom_f->GetShader().use();
	bloom_f->GetShader().uniformFloat(intensity_uid, 1.0);

	bloom_f->use(true); // SET F TO O
	bloom_o->draw();
	bloom_h->use(true); // SET H TEXTURE TO F
	bloom_f->draw();
	bloom_v->use(true); // SET V TEXTURE TO F
	bloom_f->draw();

	if (size > 0) {
		// DOWNSCALING
		for (int i = 0; i < this->mipLevels * 2; i++)
		{
			if (i % 2 == 0) {
				bloom_v->use(true); // SAVE TO SAVE BUFFER
				bloom_h->draw(); // SET DATA ON SAVE BUFFER
			}
			else {
				bloom_h->use(true);
				bloom_h->SetTexture(mips[(i - 1) / 2]);
				bloom_v->draw(); // RETURN OUTPUT DATA TO BUFFER
			}
		}

		bloom_f->use(true);
		bloom_h->draw();

	}
	bloom_f->GetShader().use();
	bloom_f->GetShader().uniformFloat(intensity_uid, this->size);
	bloom_f->draw();

	if (include_objects)
		bloom_o->draw();
};

void Bloom::Bloom_Framebuffer::DrawToDefault(glm::vec2 window_size,bool include_objects) {
	bloom_f->GetShader().use();
	bloom_f->GetShader().uniformFloat(intensity_uid, 1.0);

	bloom_f->use(true); // SET F TO O
	bloom_o->draw();

	bloom_h->use(true); // SET H TEXTURE TO F
	bloom_f->draw();
	bloom_v->use(true); // SET V TEXTURE TO F
	bloom_f->draw();

	if (size > 0) {
		// DOWNSCALING
		for (int i = 0; i < this->mipLevels * 2; i++)
		{
			if (i % 2 == 0) {
				bloom_v->use(true); // SAVE TO SAVE BUFFER
				bloom_h->draw(); // SET DATA ON SAVE BUFFER
			}
			else {
				bloom_h->use(true);
				bloom_h->SetTexture(mips[(i - 1) / 2]);
				bloom_v->draw(); // RETURN OUTPUT DATA TO BUFFER
			}
		}

		bloom_f->use(true);
		bloom_h->draw();

	}

	bloom_f->GetShader().use();
	bloom_f->GetShader().uniformFloat(intensity_uid, this->size);
	bloom_f->drawToDefault(window_size);
	
	if(include_objects)
		bloom_o->draw();
}

void Bloom::Bloom_Framebuffer::Use(bool clear) {
	bloom_o->use(clear);
}