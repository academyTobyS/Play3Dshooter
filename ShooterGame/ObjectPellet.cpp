#include "ObjectPellet.h"
#include "ObjectManager.h"
using namespace Play3d;

ObjectPellet::ObjectPellet(Play3d::Vector3f position) : GameObject(TYPE_PLAYER_PELLET, position)
{
	m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\pellet.obj");
	m_materialId = GetObjectManager()->GetMaterialHLSL("..\\Assets\\Shaders\\PlayerPellet.hlsl");

	m_colliders[0].radius = 0.1f;
}

void ObjectPellet::Update()
{
	m_lifetime -= System::GetDeltaTime();
	if (m_lifetime < 0)
	{
		Destroy();
	}

	//if()
}

void ObjectPellet::OnCollision(GameObject* other)
{
	if (other->GetObjectType() == GameObjectType::TYPE_BOSS)
	{
		Destroy();
	}
}