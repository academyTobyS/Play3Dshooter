#include "UtilityFunctions.h"

using namespace Play3d;
using namespace Graphics;

float RandValueInRange(const float min, const float max)
{
	PLAY_ASSERT(min <= max);
	return min + ((max - min) * (static_cast<float>(rand()) / RAND_MAX));
}

static const float s_viewBoundsHalf{ 15.f / 2 };
float GetGameHalfWidth()
{
	Graphics::SurfaceSize surfaceSize = Graphics::GetDisplaySurfaceSize();
	f32 aspect((f32)surfaceSize.m_width / (f32)surfaceSize.m_height);
	return s_viewBoundsHalf * aspect;
}

float GetGameHalfHeight()
{
	return s_viewBoundsHalf;
}