#include "ObjectBossPellet.h"
using namespace Play3d;

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

ObjectBossPellet::ObjectBossPellet(Play3d::Vector3f position) : GameObject(TYPE_BOSS_PELLET, position)
{
	m_meshId = AssignMesh(s_meshId, "..\\Assets\\Models\\pelletEnemy.obj");
	m_materialId = AssignMaterial(s_materialId);
}

void ObjectBossPellet::Update()
{
	if(m_pos.y < -2.f)
	{
		Destroy();
	}
}