#pragma once
#include "Flowstate.h"
#include "ParticleEmitter.h"

class FlowstateGame : public Flowstate
{
public:
	void EnterState() override;
	void ExitState() override;

	eFlowstates Update() override;
	void Draw() override;

private:
	void SetGameCamera();

	ParticleEmitter m_starEmitter;
	float m_endgameTimer{0.f};
	bool m_debugCam{false};
	bool m_debugCollision{false};
};