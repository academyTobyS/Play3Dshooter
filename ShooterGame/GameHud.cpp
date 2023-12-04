#include "GameHud.h"
#include "UtilityFunctions.h"

using namespace Play3d;

static Graphics::MaterialId s_matHud{};
static Graphics::MaterialId s_matBackground{};

GameHud::GameHud()
{
	m_meshFullscreen = Graphics::CreatePlane(GetGameHalfWidth(), GetGameHalfHeight());

	AssignMaterialHLSL(s_matHud, "..\\Assets\\Shaders\\HUD.hlsl", "..\\Assets\\Images\\HUD.png");
	AssignMaterial(s_matBackground, "..\\Assets\\Images\\background.png");
}

void GameHud::SetLives(int count)
{

}

void GameHud::SetScore(int score)
{

}

void GameHud::Draw()
{
	const float bgDepth{50.f};
	const float hudDepth{-9.9f};

	// Background texture
	Graphics::SetMaterial(s_matBackground);
	Graphics::DrawMesh(m_meshFullscreen, MatrixTranslate(0.f, 0.f, bgDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));

	// Foreground HUD texture
	Graphics::SetMaterial(s_matHud);
	Graphics::DrawMesh(m_meshFullscreen, MatrixTranslate(0.f, 0.f, hudDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));

}