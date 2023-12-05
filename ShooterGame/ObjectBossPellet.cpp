#include "ObjectBossPellet.h"
#include "ObjectManager.h"
using namespace Play3d;

ObjectBossPellet::ObjectBossPellet(Play3d::Vector3f position) : GameObject(TYPE_BOSS_PELLET, position)
{
	m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\pelletEnemy.obj");
	m_materialId = GetObjectManager()->GetMaterialHLSL("..\\Assets\\Shaders\\BossPellet.hlsl");

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