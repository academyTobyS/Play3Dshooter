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
	// If we need to assert and/or log our code there some handy macros.
	// PLAY_ASSERT()
	// PLAY_ASSERT_MSG(false, "errcode = 0x%x", 1234);
	// PLAY_TRACE("Hello Play3d");

	// First we initialise the Play3d library.
	System::Initialise();

	// We can create Simple models to render directly from code.
	// A number of simple primitive generators are provided.
	Graphics::MeshId planeId = Graphics::CreatePlane(5, 5, Colour::Darkslategray, 10.f);
	Graphics::MeshId cubeId = Graphics::CreateMeshCube(0.25f);
	Graphics::MeshId boxId = Graphics::CreateMeshBox(1.0f, 0.1f, 2.0f);
	Graphics::MeshId cylinderId = Graphics::CreateCylinder(1.0f, 0.25f, 16);
	Graphics::MeshId sphereId = Graphics::CreateSphere(0.25f, 32, 32);
	 
	Graphics::MeshId tetraHedronId = Graphics::CreatePlatonicTetrahedron();
	Graphics::MeshId hexaHedronId = Graphics::CreatePlatonicHexahedron();
	Graphics::MeshId octaHedronId = Graphics::CreatePlatonicOctahedron();

	// We can also load a mesh from an .obj file created with a 3d design tool such as blender.
	Graphics::MeshId objMeshId = Graphics::CreateMeshFromObjFile("..\\TestAssets\\icosphere.obj", Colour::White, 0.25f);

	// When we render these we need materials to define colour and lighting properties.
	Graphics::MaterialId wireframeMaterial;
	{
		Graphics::SimpleMaterialDesc desc;
		desc.m_state.m_cullMode = Graphics::CullMode::NONE;
		desc.m_state.m_fillMode = Graphics::FillMode::WIREFRAME;
		Colour::Seagreen.as_float_rgba_srgb(&desc.m_constants.diffuseColour.x);
		wireframeMaterial = Resources::CreateAsset<Graphics::Material>(desc);
	}

	// Materials can make use of basic lighting
	Graphics::MaterialId solidMaterial;
	{
		Graphics::SimpleMaterialDesc desc;
		desc.m_state.m_cullMode = Graphics::CullMode::BACK;
		desc.m_state.m_fillMode = Graphics::FillMode::SOLID;
		Colour::White.as_float_rgba_srgb(&desc.m_constants.diffuseColour.x);

		// Use shaders with lighting enabled and set the maximum number of lights.
		desc.m_bEnableLighting = true; 
		desc.m_lightCount = 3;

		solidMaterial = Resources::CreateAsset<Graphics::Material>(desc);
	}

	// Materials can make use of basic lighting
	Graphics::MaterialId solidTexturedMaterial;
	{
		Graphics::SimpleMaterialDesc desc;
		desc.m_state.m_cullMode = Graphics::CullMode::BACK;
		desc.m_state.m_fillMode = Graphics::FillMode::SOLID;
		Colour::White.as_float_rgba_srgb(&desc.m_constants.diffuseColour.x);
		desc.m_bEnableLighting = true;
		desc.m_lightCount = 3;
		
		// We can use textures.
		desc.m_texture[0] = Graphics::CreateTextureFromFile("Data/sumo_logo.png");
		desc.m_sampler[0] = Graphics::CreateLinearSampler();

		solidTexturedMaterial = Resources::CreateAsset<Graphics::Material>(desc);
	}

	// Advanced users may wish to use custom HLSL shaders in their materials.
	Graphics::MaterialId complexMaterialId = Demo::CreateComplexMaterial();

	// To use audio we have to load sound resources.
	Audio::SoundId explodeSoundId = Audio::LoadSoundFromFile("Data/explode.wav");
	Audio::SoundId musicSoundId = Audio::LoadSoundFromFile("Data/music.wav");

	// A sound instance that is playing is called a voice.
	// Lets keep a VoiceId handy for the music.
	Audio::VoiceId musicVoiceId;


	//////////////////////////////////////
	// create + register states
	//////////////////////////////////////
	FlowstateMachine states;

	FlowstateMenu stateMenu;
	states.RegisterState(&stateMenu, eFlowstates::STATE_MENU);

	FlowstateGame stateGame;
	states.RegisterState(&stateGame, eFlowstates::STATE_PLAY);
	states.SetInitialState(eFlowstates::STATE_PLAY);

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

void UnusedCode()
{
	//// Press M to toggle music.
	//if (Input::IsKeyPressed('M'))
	//{
	//	if (musicVoiceId.IsInvalid())
	//	{
	//		// For the music we want to store the VoiceId so we can stop the voice later.
	//		musicVoiceId = Audio::PlaySound(musicSoundId, 1.0f, 0.5f);
	//	}
	//	else
	//	{
	//		Audio::StopSound(musicVoiceId);
	//		musicVoiceId.Invalidate();
	//	}
	//}

	//// Press X to trigger the eXplode sound
	//if (Input::IsKeyPressed('X'))
	//{
	//	// We don't need to hang on to a VoiceId for one-shot sounds.
	//	Audio::PlaySound(explodeSoundId, 1.0f, (f32)rand() / RAND_MAX);
	//}

	//// A simple camera is provided in the demo code.
	//// You can use 'WASD' 'Q' and 'E' while dragging with the right mouse button to move and aim it.
	//// This function updates the camera using user input.
	//Demo::UpdateDebugCamera();

	//// The camera code produces 'view' and 'projection' matrices which must be passed to the GPU.
	//// There are many ways to construct and control a camera.
	//Demo::SetDebugCameraMatrices();

	//// The 'Primitive Drawing Interface' collects lines points and triangles in batches and then draws them in one go.
	//// This is suitable for debugging vector maths or testing gameplay features.
	//Graphics::BeginPrimitiveBatch();

	//// Make a simple animated value.
	//static f32 fTime = 0.f;
	//fTime += System::GetDeltaTime();

	//// Draw a field on points.
	//{
	//	f32 t = fTime;
	//	const float fScale = 0.025f;
	//	const int dim = 50;
	//	for (int i = -dim; i <= dim; ++i)
	//		for (int j = -dim; j <= dim; ++j)
	//			Graphics::DrawPoint(Vector3f((f32)i * fScale, sin(t + (f32)(i * j) * 0.01f) * 0.05f, (f32)j * fScale), Colour::White);
	//}

	//// Some lines
	//Graphics::DrawLine(Vector3f(0, 0, 0), Vector3f(1, 0, 0), Colour::Red);
	//Graphics::DrawLine(Vector3f(0, 0, 0), Vector3f(0, 1, 0), Colour::Green);
	//Graphics::DrawLine(Vector3f(0, 0, 0), Vector3f(0, 0, 1), Colour::Blue);

	//// Triangles and Quads.
	//{
	//	const float s = 0.5f;
	//	Graphics::DrawTriangle(Vector3f(s, 0, 0), Vector3f(0.f, 0, s), Vector3f(0, s, 0), Colour::Red, Colour::Blue, Colour::Green);
	//	Graphics::DrawQuad(Vector3f(-s, 0, -s), Vector3f(s, 0, -s), Vector3f(-s, 0, s), Vector3f(s, 0, s), Colour::Red, Colour::Blue, Colour::Green, Colour::Yellow);
	//}

	//// Finish the batch to pass these calls to the GPU.
	//Graphics::EndPrimitiveBatch();

	//// We can collect related primitives into further batches.
	//Demo::DrawDebugGrid();

	//// The 'Mesh Drawing Interface' allows us to draw instances of the mesh shapes created earlier.

	//// Each mesh instance is given a World transform matrix.
	//Matrix4x4f transform = MatrixTranslate<f32>(0.f, 1.f, 0.f) * MatrixRotationY<f32>(fTime * kfTwoPi); // *MatrixScale<f32>(0.5f, 0.75f, 0.24f);

	//// We can set a material then draw several meshes.
	//Graphics::SetMaterial(wireframeMaterial);
	//Graphics::DrawMesh(cubeId, transform);

	//// Platonic Solids
	//Matrix4x4f matScale = MatrixRotationY<f32>(fTime * 0.1f) * MatrixScale<f32>(0.25, 0.25, 0.25);
	//Graphics::SetMaterial(solidMaterial);
	//Graphics::DrawMesh(cubeId, transform);
	//Graphics::DrawMesh(tetraHedronId, MatrixTranslate<f32>(-1.f, 0.5f, 0.f) * matScale);
	////Graphics::SetMaterial(wireframeMaterial);
	//Graphics::DrawMesh(octaHedronId, MatrixTranslate<f32>(0.5f, 0.5f, 0.f) * matScale);
	//Graphics::DrawMesh(cylinderId, MatrixTranslate<f32>(0.75f, 0.5f, 0.f) * matScale);
	//Graphics::DrawMesh(sphereId, MatrixTranslate<f32>(0.75f, 0.5f, 1.f));

	//Graphics::DrawMesh(objMeshId, transform);

	//Graphics::SetMaterial(complexMaterialId);
	//Graphics::DrawMesh(boxId, MatrixTranslate<f32>(0.f, 0.1f, -2.f));

	//// Switch to the textured material
	//Graphics::SetMaterial(solidTexturedMaterial);
	//Graphics::DrawMesh(planeId, MatrixTranslate<f32>(0.f, -0.001f, 0.0f));
	//Graphics::DrawMesh(hexaHedronId, MatrixTranslate<f32>(-0.5f, 0.5f, 0.f) * matScale);
}