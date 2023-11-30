#include "Play3d.h"
#include "FlowstateGame.h"
#include "ObjectManager.h"

using namespace Play3d;

void FlowstateGame::EnterState()
{
	// The lighting interface allows us to set some light properties.
	// This could be animated during a frame.
	// Here we use using 3 lights in differing directions Red, Green and Blue
	Graphics::SetLightColour(0, ColourValue(0xFF0000));
	Graphics::SetLightDirection(0, Vector3f(1, 1, 1));
	Graphics::SetLightColour(1, ColourValue(0x00FF00));
	Graphics::SetLightDirection(1, Vector3f(1, 1, -1));
	Graphics::SetLightColour(2, ColourValue(0x0000FF));
	Graphics::SetLightDirection(2, Vector3f(-1, 1, -1));

	// Setup player
	GameObjectManager* pObjs{ GetObjectManager() };
	GameObject* pPlayer = pObjs->CreateObject(GameObjectType::TYPE_PLAYER, Vector3f(0.f, 0.f, 0.f));
	pObjs->SetPlayer(pPlayer);

	// Setup stars
	ParticleEmitterSettings s;
	s.capacity = 500;
	s.emitterMinExtents = Vector3f(-6.f, 0.f, -0.6f);
	s.emitterMaxExtents = Vector3f(6.f, 0.f, 0.6f);
	s.particleMinVelocity.y = -0.9f;
	s.particleMaxVelocity.y = -0.4f;
	s.particlesRelativeToEmitter = true;
	s.particleLifetime = 20.f;
	m_emitter.ApplySettings(s);
	m_emitter.m_position.y = 5.f;
	m_emitter.Prewarm();
}


eFlowstates FlowstateGame::Update()
{
	GameObjectManager* pObjs{ GetObjectManager() };

	Demo::UpdateDebugCamera();
	Demo::SetDebugCameraMatrices();
	pObjs->UpdateAll();
	m_emitter.Tick();

	m_emitter.m_position.x = (-pObjs->GetPlayer()->GetPosition().x) * 0.25f;

	return eFlowstates::STATE_NULL;
}


void FlowstateGame::Draw()
{
	Graphics::BeginPrimitiveBatch();

	//Demo::DrawDebugGrid();
	GetObjectManager()->DrawAll();
	m_emitter.Draw();


	// Debug Text
	UI::FontId fontId = UI::GetDebugFont();
	static u32 frameCounter = 0;
	UI::DrawString(fontId, Vector2f(20, 20), Colour::White, "Play3d, Single Header DX11");
	UI::DrawPrintf(fontId, Vector2f(20, 50), Colour::Lightblue, "[frame %d, delta=%.2fms elapsed=%.2fs]", frameCounter++, System::GetDeltaTime() * 1000.f, System::GetElapsedTime());

	Graphics::EndPrimitiveBatch();
}

void FlowstateGame::ExitState()
{

}