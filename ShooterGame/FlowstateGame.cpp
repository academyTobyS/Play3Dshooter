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

	GetObjectManager()->CreateObject(GameObjectType::TYPE_PLAYER, Vector2f(20.f, 100.f));
}


eFlowstates FlowstateGame::Update()
{
	Demo::UpdateDebugCamera();
	Demo::SetDebugCameraMatrices();
	GetObjectManager()->UpdateAll();
	return eFlowstates::STATE_NULL;
}


void FlowstateGame::Draw()
{
	GetObjectManager()->DrawAll();

	// Debug Text
	UI::FontId fontId = UI::GetDebugFont();
	static u32 frameCounter = 0;
	UI::DrawString(fontId, Vector2f(20, 20), Colour::White, "Play3d, Single Header DX11");
	UI::DrawPrintf(fontId, Vector2f(20, 50), Colour::Lightblue, "[frame %d, delta=%.2fms elapsed=%.2fs]", frameCounter++, System::GetDeltaTime() * 1000.f, System::GetElapsedTime());
}


void FlowstateGame::ExitState()
{

}