#pragma once
#include "Flowstate.h"
#include "ParticleEmitter.h"
#include "MenuShip.h"
#include "MenuButton.h"

class FlowstateMenu : public Flowstate
{
public:
	void EnterState() override;
	void ExitState() override;

	eFlowstates Update() override;
	void Draw() override;

private:
	void SetMenuSceneCamera();
	void SetMenuOrthoCamera();

	ParticleEmitter m_starEmitter;
	MenuShip m_menuShip;
	MenuButton m_buttonPlay;

	bool m_debugCam{false};
};