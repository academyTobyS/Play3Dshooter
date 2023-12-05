#pragma once
#include "Play3d.h"

float RandValueInRange(const float min, const float max);

// Assuming 0 is middle of screen, returns distance to horiz/vertical edge (when using ortho projection)
float GetGameHalfWidth();
float GetGameHalfHeight();
