#include "GameHud.h"
#include "UtilityFunctions.h"
#include "ObjectManager.h"

using namespace Play3d;

static GameHud* s_pHud{nullptr};

GameHud* GameHud::Get()
{
	if (!s_pHud)
	{
		s_pHud = new GameHud();
	}
	return s_pHud;
}

void GameHud::Destroy()
{
	delete s_pHud;
	s_pHud = nullptr;
}

GameHud::GameHud()
{
	m_meshFullscreen = Graphics::CreatePlane(GetGameHalfWidth(), GetGameHalfHeight());
	m_meshIcon = Graphics::CreatePlane(.5f, .5f);

	m_matHud = GetObjectManager()->GetMaterialHLSL("..\\Assets\\Shaders\\HUD.hlsl", "..\\Assets\\Images\\HUD.png");
	m_matBackground = GetObjectManager()->GetMaterial("..\\Assets\\Images\\background.png");
	m_matLife = GetObjectManager()->GetMaterial("..\\Assets\\Images\\life.png");
	m_matBomb = GetObjectManager()->GetMaterial("..\\Assets\\Images\\bomb.png");
}

void GameHud::Draw()
{
	const float bgDepth{50.f};
	float hudDepth{-9.0f};

	// Background texture
	Graphics::SetMaterial(m_matBackground);
	Graphics::DrawMesh(m_meshFullscreen, MatrixTranslate(0.f, 0.f, bgDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));
	// Foreground HUD texture
	Graphics::SetMaterial(m_matHud);
	Graphics::DrawMesh(m_meshFullscreen, MatrixTranslate(0.f, 0.f, hudDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));
	// Update hud depth for items on top
	hudDepth -= 0.1f;

	float iconSpacing{1.5f};

	// Lives
	Graphics::SetMaterial(m_matLife);
	Vector2f lifeOrigin{11.6f, 4.f};
	for (int i = 0; i < m_lives; i++)
	{
		Graphics::DrawMesh(m_meshIcon, MatrixTranslate(lifeOrigin.x, lifeOrigin.y, hudDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));
		lifeOrigin.y -= iconSpacing;
	}

	// Bombs
	Graphics::SetMaterial(m_matBomb);
	Vector2f bombOrigin{ 11.6f, -4.f };
	for (int i = 0; i < m_bombs; i++)
	{
		Graphics::DrawMesh(m_meshIcon, MatrixTranslate(bombOrigin.x, bombOrigin.y, hudDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));
		bombOrigin.y -= iconSpacing;
	}

	// Boss Health
	Vector3f healthMid{-GetGameHalfWidth() + 1.8f, -6.2f, hudDepth};
	Vector3f healthExtent{.5f, 10.f, 0.f};
	Graphics::DrawQuad( // OUTLINE
		healthMid + Vector3f(healthExtent.x, healthExtent.y, 0.f),
		healthMid + Vector3f(-healthExtent.x, healthExtent.y, 0.f),
		healthMid + Vector3f(healthExtent.x, 0.f, 0.f),
		healthMid + Vector3f(-healthExtent.x, 0.f, 0.f),
		Colour::White
	);
	healthMid.z -= 0.1f;
	float trimY{0.08f};
	float trimX{0.05f};
	healthExtent.x -= trimY; // shrink for outline to be visible
	healthExtent.y -= trimX; // shrink for outline to be visible
	Graphics::DrawQuad( // HEALTH
		healthMid + Vector3f(healthExtent.x, healthExtent.y * m_bossHealthPercent, 0.f),
		healthMid + Vector3f(-healthExtent.x, healthExtent.y * m_bossHealthPercent, 0.f),
		healthMid + Vector3f(healthExtent.x, trimY, 0.f),
		healthMid + Vector3f(-healthExtent.x, trimY, 0.f),
		Colour::Red
	);
}