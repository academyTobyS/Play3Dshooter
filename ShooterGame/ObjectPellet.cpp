#include "ObjectPellet.h"
using namespace Play3d;

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

ObjectPellet::ObjectPellet(Play3d::Vector3f position) : GameObject(TYPE_PLAYER_PELLET, position)
{
	m_meshId = AssignMesh(s_meshId, "..\\Assets\\Models\\pellet.obj");
	m_materialId = AssignMaterialHLSL(s_materialId, "..\\Assets\\Shaders\\PlayerPellet.hlsl");
}

void ObjectPellet::Update()
{
	m_lifetime -= System::GetDeltaTime();
	if (m_lifetime < 0)
	{
		Destroy();
	}
}