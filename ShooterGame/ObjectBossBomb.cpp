#include "ObjectBossBomb.h"
#include "ObjectManager.h"
using namespace Play3d;

ObjectBossBomb::ObjectBossBomb(Play3d::Vector3f position) : GameObject(TYPE_BOSS_PELLET, position)
{
	m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\pelletEnemy.obj");
	m_materialId = GetObjectManager()->GetMaterialHLSL("..\\Assets\\Shaders\\BossPellet.hlsl");

	m_scale = 2.f;
	m_colliders[0].radius = 0.15f;
}

void ObjectBossBomb::Update()
{
	m_detonationTimer -= Play3d::System::GetDeltaTime();
	if (m_detonationTimer <= 0.f)
	{
		Burst();
	}
}

void ObjectBossBomb::OnCollision(GameObject* other)
{
	if (other->GetObjectType() == GameObjectType::TYPE_PLAYER)
	{
		Burst();
	}
}

void ObjectBossBomb::Burst()
{
	Destroy();

	float rotIncrement{kfTwoPi / m_fragmentTotal};

	GameObjectManager* pObjs{GetObjectManager()};
	for (int i = 0; i < m_fragmentTotal; i++)
	{
		GameObject* pPellet = pObjs->CreateObject(TYPE_BOSS_PELLET, m_pos);

		float x = sin(i * rotIncrement);
		float y = cos(i * rotIncrement);
		if (fabs(y) < std::numeric_limits<float>::epsilon())
		{
			y = 0.f;
		}
		if (fabs(x) < std::numeric_limits<float>::epsilon())
		{
			x = 0.f;
		}
		Vector3f direction{x, y, 0.f};

		pPellet->SetVelocity(direction * 0.05f);
		pPellet->SetHidden(false);
	}
}