#include "FlowstateMachine.h"
#include "Play3d.h"

void FlowstateMachine::RegisterState(Flowstate* pState, eFlowstates stateId)
{
	PLAY_ASSERT(stateId >= 0 && stateId < eFlowstates::STATE_TOTAL);
	PLAY_ASSERT(pState);
	m_states[stateId] = pState;
}

void FlowstateMachine::SetInitialState(eFlowstates stateId)
{
	PLAY_ASSERT(stateId >= 0 && stateId < eFlowstates::STATE_TOTAL);
	m_curState = stateId;

	PLAY_ASSERT(m_states[m_curState] != nullptr);
	m_states[m_curState]->EnterState();
}

void FlowstateMachine::Update()
{
	PLAY_ASSERT(m_curState != eFlowstates::STATE_NULL);

	eFlowstates nextState = m_states[m_curState]->Update();
	m_states[m_curState]->Draw();

	if (nextState != eFlowstates::STATE_NULL)
	{
		PLAY_ASSERT(m_states[nextState] != nullptr);

		m_states[m_curState]->ExitState();
		m_curState = nextState;
		m_states[m_curState]->EnterState();
	}
}