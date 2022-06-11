#include "shake.h"

using namespace Bloom;

Shake::Shake(float frequency, float amplitude, float duration, int randomness ,bool continuous) {
	this->freq = frequency; this->ampl = amplitude; this->dura = duration; this->cont = continuous;
	if (randomness != 0)
		this->rnd = ((rand() % randomness * 2) - randomness);
}

float Shake::GetValue(float time) {
	if (!active)
		return 0;
	time = (time - this->time);
	if (time >= dura && !cont)
		return 0;
	return ampl * sin(time * freq*(3.1416/dura)) * ((cont) ? 1 : (1 - time / dura)) * ((sin(rnd/ freq * time) != 0) ? rnd : 1);
}