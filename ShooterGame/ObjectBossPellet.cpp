#include "ObjectBossPellet.h"
using namespace Play3d;

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

ObjectBossPellet::ObjectBossPellet(Play3d::Vector3f position) : GameObject(TYPE_BOSS_PELLET, position)
{
	m_meshId = AssignMesh(s_meshId, "..\\Assets\\Models\\pelletEnemy.obj");
	m_materialId = AssignMaterialHLSL(s_materialId, "..\\Assets\\Shaders\\BossPellet.hlsl");

	m_collisionRadius = 0.15f;
}

void ObjectBossPellet::Update()
{
	if(m_pos.y < -2.f)
	{
		Destroy();
	}
}

void ObjectBossPellet::OnCollision(GameObject* other)
{
	if(other->GetObjectType() == GameObjectType::TYPE_PLAYER)
	{
		Destroy();
	}
}