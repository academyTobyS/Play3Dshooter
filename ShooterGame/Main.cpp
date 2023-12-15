///////////////////////////////////////////////////////////////////////////
//	Copyright (C) Sumo Digital Ltd. All rights reserved.
///////////////////////////////////////////////////////////////////////////

// We have to define this in one .cpp only to bring in our implementation code.
// It must be defined just before the #include "Play3d.h"
#define PLAY_IMPLEMENTATION
#include "Play3d.h"

#include "FlowstateMachine.h"
#include "FlowstateMenu.h"
#include "FlowstateGame.h"

// Play3d uses namespaces for each area of code.
// The top level namespace is Play3d
using namespace Play3d;

int PlayMain()
{
	// First we initialise the Play3d library.
	System::Initialise();

	//////////////////////////////////////
	// create + register states
	//////////////////////////////////////
	FlowstateMachine states;

	FlowstateMenu stateMenu;
	states.RegisterState(&stateMenu, eFlowstates::STATE_MENU);

	FlowstateGame stateGame;
	states.RegisterState(&stateGame, eFlowstates::STATE_PLAY);

	states.SetInitialState(eFlowstates::STATE_MENU);
	//////////////////////////////////////
	// main game loop
	//////////////////////////////////////
	bool bKeepGoing = true;
	while (bKeepGoing)
	{
		// BeginFrame should be called first, it will return RESULT_QUIT if the user has quit via 'Close' icons.
		if (System::BeginFrame() != RESULT_OK || Input::IsKeyPressed(VK_ESCAPE))
		{
			bKeepGoing = false;
		}

		states.Update();

		// Finally we signal the framework to finish the frame.
		System::EndFrame();
	}

	// Make sure to shutdown the library before we end our main function.
	System::Shutdown();

	return 0;
};
