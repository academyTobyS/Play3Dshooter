#pragma once
#include "Flowstate.h"

class FlowstateMenu : public Flowstate
{
public:
	void EnterState() override;
	void ExitState() override;

	eFlowstates Update() override;
	void Draw() override;
};