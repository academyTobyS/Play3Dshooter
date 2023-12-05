#pragma once
#include "Play3d.h"

class GameHud
{
public:
	GameHud();

	void SetLives(int count);
	void SetScore(int score);
	void Draw();

private:
	Play3d::Graphics::MeshId m_meshFullscreen;
	Play3d::Graphics::MaterialId m_matHud;
	Play3d::Graphics::MaterialId m_matBackground;
};

