#include "Play3d.h"
#include "FlowstateGame.h"
#include "ObjectManager.h"
#include "UtilityFunctions.h"
#include "GameHud.h"
using namespace Play3d;

void FlowstateGame::EnterState()
{
	// The lighting interface allows us to set some light properties.
	// This could be animated during a frame.
	// Here we use using 3 lights in differing directions Red, Green and Blue
	Graphics::SetLightColour(0, ColourValue(0xFFFFFF));
	Graphics::SetLightDirection(0, Vector3f(1, 1, 1));
	Graphics::SetLightColour(1, ColourValue(0xFFFFFF));
	Graphics::SetLightDirection(1, Vector3f(1, 1, -1));
	Graphics::SetLightColour(2, ColourValue(0xFFFFFF));
	Graphics::SetLightDirection(2, Vector3f(-1, 1, -1));

	// Setup player
	GameObjectManager* pObjs{ GetObjectManager() };
	GameObject* pPlayer = pObjs->CreateObject(GameObjectType::TYPE_PLAYER, Vector3f(0.f, -GetGameHalfHeight() / 1.25f, 0.f));
	pObjs->SetPlayer(pPlayer);

	pObjs->CreateObject(GameObjectType::TYPE_BOSS, Vector3f(0.f, GetGameHalfHeight() / 1.25f, 0.f));

	// Setup stars
	ParticleEmitterSettings s;
	s.capacity = 400;
	s.emitterMinExtents = Vector3f(-10.f, 0.f, -0.6f);
	s.emitterMaxExtents = Vector3f(10.f, 0.f, 0.6f);
	s.particleMinVelocity.y = -5.f;
	s.particleMaxVelocity.y = -3.f;
	s.emitWaitMin = 0.01f;
	s.emitWaitMax = 0.1f;
	s.particlesRelativeToEmitter = true;
	s.particleLifetime = 6.f;
	s.particlesPerEmit = 4;
	m_starEmitter.ApplySettings(s);
	m_starEmitter.m_position.y = 10.f;
	m_starEmitter.Prewarm();

	SetGameCamera();
}

void FlowstateGame::SetGameCamera()
{
	Graphics::SurfaceSize surfaceSize = Graphics::GetDisplaySurfaceSize();

	// Perspective View
	/*
	//f32 fovY(kfPi / 4.f), aspect((f32)surfaceSize.m_width / (f32)surfaceSize.m_height), nearZ(0.1f), farZ(15.f);
	Vector3f camEye{0.f, 2.5f, -10.f};
	Vector3f forward{0.f, 0.f, 1.f};
	Vector3f up{0.f, 1.f, 0.f};
	Matrix4x4f view = MatrixLookatRH(camEye, camEye + forward, up);
	Matrix4x4f project = MatrixPerspectiveProjectRH(fovY, aspect, nearZ, farZ);
	*/
	
	// Ortho View
	Vector3f camEye{ 0.f, 0.f, -10.f};
	Vector3f forward{ 0.f, 0.f, 1.f };
	Vector3f up{ 0.f, 1.f, 0.f };
	Matrix4x4f view = MatrixLookatRH(camEye, camEye + forward, up);
	Matrix4x4f projectOrtho = MatrixOrthoProjectRH(-GetGameHalfWidth(), GetGameHalfWidth(), -GetGameHalfHeight(), GetGameHalfHeight(), 0.f, 100.f);

	Graphics::SetViewport(Graphics::Viewport(surfaceSize));
	Graphics::SetViewMatrix(view);
	Graphics::SetProjectionMatrix(projectOrtho);
}

eFlowstates FlowstateGame::Update()
{
	if (Input::IsKeyPressed(VK_F1))
	{
		m_debugCam = !m_debugCam;
	}
	if (Input::IsKeyPressed(VK_F2))
	{
		m_debugCollision = !m_debugCollision;
	}

	if(m_debugCam)
	{
		Demo::UpdateDebugCamera();
		Demo::SetDebugCameraMatrices();
	}
	else
	{
		SetGameCamera();
	}

	GameObjectManager* pObjs{ GetObjectManager() };
	pObjs->UpdateAll();
	m_starEmitter.Tick();

	return eFlowstates::STATE_NULL;
}


void FlowstateGame::Draw()
{
	if (m_debugCam)
	{
		Demo::DrawDebugGrid();
		UI::FontId fontId = UI::GetDebugFont();
		static u32 frameCounter = 0;
		UI::DrawString(fontId, Vector2f(20, 20), Colour::White, "Play3d, Single Header DX11");
		UI::DrawPrintf(fontId, Vector2f(20, 50), Colour::Lightblue, "[frame %d, delta=%.2fms elapsed=%.2fs]", frameCounter++, System::GetDeltaTime() * 1000.f, System::GetElapsedTime());
	}


	Graphics::BeginPrimitiveBatch();
	if (m_debugCollision)
	{
		GetObjectManager()->DrawCollisionAll();
	}
	GetObjectManager()->DrawAll();
	m_starEmitter.Draw();
	GameHud::Get()->Draw();
	Graphics::EndPrimitiveBatch();

}

void FlowstateGame::ExitState()
{
	DestroyObjectManager();
	GameHud::Destroy();
}