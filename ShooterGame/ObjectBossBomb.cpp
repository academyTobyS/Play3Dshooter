#include "ObjectBossBomb.h"
#include "ObjectManager.h"
using namespace Play3d;

static constexpr int BURST_TOTAL{12};
static constexpr float ROT_INCREMENT{kfTwoPi / BURST_TOTAL};

ObjectBossBomb::ObjectBossBomb(Play3d::Vector3f position) : GameObject(TYPE_BOSS_PELLET, position)
{
	m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\pelletEnemy.obj");
	m_materialId = GetObjectManager()->GetMaterialHLSL("..\\Assets\\Shaders\\BossPellet.hlsl");

	m_scale = 2.f;
	m_colliders[0].radius = 0.15f;
}

void ObjectBossBomb::Update()
{
	if (m_pos.y < -2.5f)
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

	GameObjectManager* pObjs{GetObjectManager()};
	for (int i = 0; i < BURST_TOTAL; i++)
	{
		GameObject* pPellet = pObjs->CreateObject(TYPE_BOSS_PELLET, m_pos);

		float x = sin(i * ROT_INCREMENT);
		float y = cos(i * ROT_INCREMENT);
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