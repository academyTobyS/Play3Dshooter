#include "RandUtility.h"
#include "Play3d.h"

float RandValueInRange(const float min, const float max)
{
	PLAY_ASSERT(min <= max);
	return min + ((max - min) * (static_cast<float>(rand()) / RAND_MAX));
}