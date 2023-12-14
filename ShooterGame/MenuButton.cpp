#include "MenuButton.h"
#include "ObjectManager.h"
using namespace Play3d;

static constexpr float BUTTON_HALFEXTENT_X{100.f};
static constexpr float BUTTON_HALFEXTENT_Y{50.f};

MenuButton::MenuButton()
{
	m_mesh = Graphics::CreatePlane(BUTTON_HALFEXTENT_X, BUTTON_HALFEXTENT_Y);
}

void MenuButton::SetImages(const char* filepathUp, const char* filepathDown)
{
	m_matUp = GetObjectManager()->GetMaterial(filepathUp);
	m_matDown = GetObjectManager()->GetMaterial(filepathDown);
}

void MenuButton::Update()
{
	Play3d::Input::MouseState mouseState = Play3d::Input::GetMouseState();
	Vector2f mousePos = ScreenPosToOrtho(Vector2f(mouseState.m_x, mouseState.m_y));

	if(	   mousePos.x < m_pos.x + BUTTON_HALFEXTENT_X
		&& mousePos.x > m_pos.x - BUTTON_HALFEXTENT_X
		&& mousePos.y < m_pos.x + BUTTON_HALFEXTENT_Y
		&& mousePos.y > m_pos.x - BUTTON_HALFEXTENT_Y)
	{
		m_hovered = true;
		m_clicked = mouseState.m_leftButton;
	}
	else
	{
		m_hovered = false;
		m_clicked = false;
	}
}

// assumes orthographic space is equal to display surface size
Vector2f MenuButton::ScreenPosToOrtho(const Vector2f& screenPos)
{
	Vector2f result{screenPos};
	Graphics::SurfaceSize surfaceSize = Graphics::GetDisplaySurfaceSize();
	/*result.x = screenPos.x / surfaceSize.m_width;
	result.y = screenPos.y / surfaceSize.m_height;*/
	result.x -= (surfaceSize.m_width / 2);
	result.y -= (surfaceSize.m_height / 2);
	return result;
}

void MenuButton::Draw()
{
	if (IsHovered())
	{
		Graphics::SetMaterial(m_matDown);
	}
	else
	{
		Graphics::SetMaterial(m_matUp);
	}
	Graphics::DrawMesh(m_mesh, MatrixTranslate(m_pos.x, m_pos.y, 0.f) * MatrixRotationX<f32>(kfHalfPi) * MatrixRotationZ<f32>(kfPi));
}
