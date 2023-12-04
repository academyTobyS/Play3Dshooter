#pragma once
#include "Flowstate.h"
#include "ParticleEmitter.h"
#include "GameHud.h"

class FlowstateGame : public Flowstate
{
public:
	void EnterState() override;
	void ExitState() override;

	eFlowstates Update() override;
	void Draw() override;

private:
	void SetGameCamera();

	ParticleEmitter m_emitter;
	GameHud m_hud;
	bool m_debugCam{ false };
};