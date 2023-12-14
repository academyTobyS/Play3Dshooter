#include "FlowstateMenu.h"
using namespace Play3d;

void FlowstateMenu::EnterState()
{
	// Setup play button
	m_buttonPlay.SetImages("..\\Assets\\Images\\ButtonPlay-Up.png", "..\\Assets\\Images\\ButtonPlay-Down.png");
	m_buttonPlay.m_pos.y = 2.5f;

	// Setup stars
	ParticleEmitterSettings s;
	s.capacity = 400;
	s.emitterMinExtents = Vector3f(-10.f, -10.f, -0.6f);
	s.emitterMaxExtents = Vector3f(10.f, 10.f, 0.6f);
	s.particleMinVelocity.z = -5.f;
	s.particleMaxVelocity.z = -3.f;
	s.emitWaitMin = 0.01f;
	s.emitWaitMax = 0.1f;
	s.particlesRelativeToEmitter = true;
	s.particleLifetime = 6.f;
	s.particlesPerEmit = 4;
	m_starEmitter.ApplySettings(s);
	m_starEmitter.m_position.z = 15.f;
	m_starEmitter.Prewarm();

	// Setup lights
	Graphics::SetLightColour(0, ColourValue(0xFFFFFF));
	Graphics::SetLightDirection(0, Vector3f(1, 1, 1));
	Graphics::SetLightColour(1, ColourValue(0xFFFFFF));
	Graphics::SetLightDirection(1, Vector3f(1, 1, -1));
	Graphics::SetLightColour(2, ColourValue(0xFFFFFF));
	Graphics::SetLightDirection(2, Vector3f(-1, 1, -1));
}

void FlowstateMenu::SetMenuSceneCamera()
{
	// Perspective View
	Graphics::SurfaceSize surfaceSize = Graphics::GetDisplaySurfaceSize();
	f32 fovY(kfPi / 4.f), aspect((f32)surfaceSize.m_width / (f32)surfaceSize.m_height), nearZ(0.1f), farZ(100.f);
	Vector3f camEye{0.f, 0.5f, -4.2f};
	Vector3f forward{0.0f, 0.1f, 1.f};
	Vector3f up{0.f, 0.f, -1.f};
	Matrix4x4f view = MatrixLookatRH(camEye, camEye + forward, up);
	Matrix4x4f project = MatrixPerspectiveProjectRH(fovY, aspect, nearZ, farZ);

	Graphics::SetViewport(Graphics::Viewport(surfaceSize));
	Graphics::SetViewMatrix(view);
	Graphics::SetProjectionMatrix(project);
}

void FlowstateMenu::SetMenuOrthoCamera()
{
	// Ortho View
	Graphics::SurfaceSize surfaceSize = Graphics::GetDisplaySurfaceSize();
	f32 halfWidth = surfaceSize.m_width / 2;
	f32 halfHeight = surfaceSize.m_height / 2;

	Vector3f camEye{ 0.f, 0.f, -10.f};
	Vector3f forward{ 0.f, 0.f, 1.f };
	Vector3f up{ 0.f, 1.f, 0.f };
	Matrix4x4f view = MatrixLookatRH(camEye, camEye + forward, up);
	Matrix4x4f projectOrtho = MatrixOrthoProjectRH(-halfWidth, halfWidth, -halfHeight, halfHeight, -10.f, 100.f);

	Graphics::SetViewport(Graphics::Viewport(surfaceSize));
	Graphics::SetViewMatrix(view);
	Graphics::SetProjectionMatrix(projectOrtho);
}

eFlowstates FlowstateMenu::Update()
{
	m_starEmitter.Tick();
	m_menuShip.Update();
	m_buttonPlay.Update();

	if (Input::IsKeyPressed(VK_F1))
	{
		m_debugCam = !m_debugCam;
	}

	if (m_buttonPlay.IsClicked())
	{
		return eFlowstates::STATE_PLAY;
	}

	return eFlowstates::STATE_NULL;
}

void FlowstateMenu::Draw()
{
	// Draw '3d world'
	if (m_debugCam)
	{
		Demo::UpdateDebugCamera();
		Demo::SetDebugCameraMatrices();
		Demo::DrawDebugGrid();
	}
	else
	{
		SetMenuSceneCamera();
	}
	Graphics::BeginPrimitiveBatch();
	m_menuShip.Draw();
	m_starEmitter.Draw();
	Graphics::EndPrimitiveBatch();

	// Draw orthographic 'menu'
	SetMenuOrthoCamera();
	m_buttonPlay.Draw();
}

void FlowstateMenu::ExitState()
{

}