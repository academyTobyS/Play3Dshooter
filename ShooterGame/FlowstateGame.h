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
	ParticleEmitter m_emitter;
};