#include "ObjectPlayer.h"

using namespace Play3d;

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

Graphics::MeshId AssignMesh()
{
	if(s_meshId.IsInvalid())
	{
		s_meshId = Graphics::CreateMeshFromObjFile("..\\Assets\\Models\\ship.obj", Colour::White, 0.25f);
	}
	return s_meshId;
}

Graphics::MaterialId AssignMaterial()
{
	if (s_materialId.IsInvalid())
	{
		Graphics::SimpleMaterialDesc desc;
		desc.m_state.m_cullMode = Graphics::CullMode::BACK;
		desc.m_state.m_fillMode = Graphics::FillMode::SOLID;
		Colour::White.as_float_rgba_srgb(&desc.m_constants.diffuseColour.x);
		desc.m_bEnableLighting = true;
		desc.m_lightCount = 3;

		// We can use textures.
		//desc.m_texture[0] = Graphics::CreateTextureFromFile("Data/sumo_logo.png");
		//desc.m_sampler[0] = Graphics::CreateLinearSampler();

		s_materialId = Resources::CreateAsset<Graphics::Material>(desc);
	}
	return s_materialId;
}

ObjectPlayer::ObjectPlayer(Vector2f position) : GameObject(TYPE_PLAYER, position)
{
	m_meshId = AssignMesh();
	m_materialId = AssignMaterial();

	// custom HLSL possible, example via:
	//Play3d::Demo::CreateComplexMaterial()
}

void ObjectPlayer::Update()
{

}

void ObjectPlayer::Draw()
{
	Matrix4x4f transform = MatrixTranslate<f32>(0.f, 0.f, 0.f) * MatrixRotationY<f32>(kfHalfPi); // *MatrixScale<f32>(0.5f, 0.75f, 0.24f);
	Graphics::SetMaterial(m_materialId);
	Graphics::DrawMesh(m_meshId, transform);
}