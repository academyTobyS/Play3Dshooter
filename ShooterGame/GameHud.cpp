#include "GameHud.h"
#include "UtilityFunctions.h"
#include "ObjectManager.h"

using namespace Play3d;

GameHud::GameHud()
{
	m_meshFullscreen = Graphics::CreatePlane(GetGameHalfWidth(), GetGameHalfHeight());

	m_matHud = GetObjectManager()->GetMaterialHLSL("..\\Assets\\Shaders\\HUD.hlsl", "..\\Assets\\Images\\HUD.png");
	m_matBackground = GetObjectManager()->GetMaterial("..\\Assets\\Images\\background.png");
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
	Graphics::SetMaterial(m_matBackground);
	Graphics::DrawMesh(m_meshFullscreen, MatrixTranslate(0.f, 0.f, bgDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));

	// Foreground HUD texture
	Graphics::SetMaterial(m_matHud);
	Graphics::DrawMesh(m_meshFullscreen, MatrixTranslate(0.f, 0.f, hudDepth) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));
}