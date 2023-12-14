#include "MenuShip.h"
#include "ObjectManager.h"
using namespace Play3d;

static constexpr float SHIP_HALFWIDTH{ 0.15f };
static constexpr float POS_Y{0.f};
static constexpr float POS_Z{0.f};
static constexpr float ROT_FORWARD{kfHalfPi * 0.75f};

MenuShip::MenuShip()
{
	GameObjectManager* pObjs{ GetObjectManager() };

	// Load and assign player mesh/mat
	m_mesh = pObjs->GetMesh("..\\Assets\\Models\\_fighter.obj");
	m_mat = pObjs->GetMaterial("..\\Assets\\Models\\_fighter-blue.jpg");
}

void MenuShip::Update()
{
	float time = Play3d::System::GetElapsedTime() * 0.8f; // reduce anim speed
	m_posX = sin(time);
	m_rotZ = sin(time - kfQuartPi) * 0.5f; // delay rotation for more 'natural' look, max rotation halved
}

void MenuShip::Draw()
{
	Matrix4x4f transform = MatrixTranslate<f32>(m_posX, POS_Y, POS_Z)
						 * MatrixRotationZ<f32>(m_rotZ)
						 * MatrixRotationX<f32>(ROT_FORWARD);

	Graphics::SetMaterial(m_mat);
	Graphics::DrawMesh(m_mesh, transform);
}