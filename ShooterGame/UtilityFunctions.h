#pragma once
#include "Play3d.h"

float RandValueInRange(const float min, const float max);
Play3d::Graphics::MeshId AssignMesh(Play3d::Graphics::MeshId& rId, const char* filepath);
Play3d::Graphics::MaterialId AssignMaterial(Play3d::Graphics::MaterialId& rId, const char* textureFilepath = "");
Play3d::Graphics::MaterialId AssignMaterialHLSL(Play3d::Graphics::MaterialId& rId, const char* hlslPath, const char* texturePath = "");

// Assuming 0 is middle of screen, returns distance to horiz/vertical edge (when using ortho projection)
float GetGameHalfWidth();
float GetGameHalfHeight();
