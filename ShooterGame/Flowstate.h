#pragma once

enum eFlowstates
{
	STATE_NULL = -1,

	STATE_MENU,
	STATE_PLAY,

	STATE_TOTAL
};

class Flowstate
{
public:
	Flowstate(){};
	~Flowstate(){};

	virtual void EnterState() = 0;
	virtual void ExitState() = 0;

	virtual eFlowstates Update() = 0;
	virtual void Draw() = 0;
};