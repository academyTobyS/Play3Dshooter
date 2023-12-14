#pragma once
#include "Play3d.h"

class MenuButton
{
public:
	MenuButton();

	void SetImages(const char* filepathUp, const char* filepathDown);
	void Update();
	void Draw();

	bool IsHovered(){return m_hovered;};
	bool IsClicked(){return m_clicked;};

	Play3d::Vector2f m_pos{0.f, 0.f};

private:
	// assumes orthographic space is equal to display surface size
	Play3d::Vector2f ScreenPosToOrtho(const Play3d::Vector2f& screenPos);

	Play3d::Graphics::MeshId m_mesh;
	Play3d::Graphics::MaterialId m_matUp;
	Play3d::Graphics::MaterialId m_matDown;
	bool m_hovered{false};
	bool m_clicked{false};
};

