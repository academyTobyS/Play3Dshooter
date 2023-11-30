#pragma once
#include "Flowstate.h"

class FlowstateMachine
{
public:
	void RegisterState(Flowstate* pState, eFlowstates stateId);
	void SetInitialState(eFlowstates stateId);
	void Update();

private:
	Flowstate* m_states[STATE_TOTAL] = {};
	eFlowstates m_curState{eFlowstates::STATE_NULL};
};