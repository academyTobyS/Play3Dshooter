#pragma once
#include "Play3d.h"

class MenuShip
{
public:
	MenuShip();

	void Update();
	void Draw();

private:
	Play3d::Graphics::MeshId m_mesh;
	Play3d::Graphics::MaterialId m_mat;

	float m_posX{0.f};
	float m_rotZ{0.f};
};