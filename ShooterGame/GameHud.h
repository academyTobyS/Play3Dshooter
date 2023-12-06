#pragma once
#include "Play3d.h"

class GameHud
{
public:
	static GameHud* Get();
	static void Destroy();

	void SetLives(int life){m_lives = life;};
	void SetBombs(int bomb){m_bombs = bomb;};
	void SetBossBarPercent(float percent){m_bossHealthPercent = percent;};
	void Draw();

private:
	GameHud();
	Play3d::Graphics::MeshId m_meshFullscreen;
	Play3d::Graphics::MeshId m_meshIcon;
	Play3d::Graphics::MaterialId m_matHud;
	Play3d::Graphics::MaterialId m_matBackground;
	Play3d::Graphics::MaterialId m_matLife;
	Play3d::Graphics::MaterialId m_matBomb;
	float m_bossHealthPercent{1.f};
	int m_lives{3};
	int m_bombs{2};
};

