#include "ObjectBoss.h"
#include "ObjectManager.h"

using namespace Play3d;

static constexpr float WOBBLE_STRENGTH{kfQuartPi / 4.f};
static constexpr float POS_LIMIT_X{8.f};

static constexpr float CANNON_SPACING{0.75f};
static constexpr float CANNON_SHOTSPEED{-0.05f};
static constexpr int TOTAL_CANNONS{7};

static constexpr float COOLDOWN_FIRE{0.5f};

ObjectBoss::ObjectBoss(Play3d::Vector3f position) : GameObject(TYPE_BOSS, position)
{
	m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_station.obj");
	m_materialId = GetObjectManager()->GetMaterial("..\\Assets\\Models\\_station-red.jpg");

	m_colliders[0].type = CollisionMode::COLL_RECT;
	m_colliders[0].extents = Vector2f(3.9f, 0.75f);
	m_colliders[0].offset.x = -1.1f;
}

void ObjectBoss::Update()
{
	float elapsedTime = System::GetElapsedTime();

	m_rotation.x = sin(elapsedTime * 2) * WOBBLE_STRENGTH / 2;
	m_rotation.y = cos(elapsedTime) * WOBBLE_STRENGTH;
	m_pos.x = sin(elapsedTime / 4) * POS_LIMIT_X;

	m_fireCooldown -= System::GetDeltaTime();
	if (m_fireCooldown < 0.f)
	{
		GameObjectManager* pObjs{GetObjectManager()};
		m_fireCooldown = COOLDOWN_FIRE;
		for (int i = 0; i < TOTAL_CANNONS; i++)
		{
			FireCannon(i);
		}
	}
}

void ObjectBoss::FireCannon(int cannonId, float speed)
{
	PLAY_ASSERT(cannonId < TOTAL_CANNONS);
	Vector3f spawnPos{ m_pos };
	spawnPos.y -= 0.8f;
	spawnPos.x -= ((f32)TOTAL_CANNONS / 2) * CANNON_SPACING;
	spawnPos.x += CANNON_SPACING * cannonId;
	spawnPos.x += 0.38f;

	// Default to CANNON_SHOTSPEED but allow custom speeds
	float shotSpeed = (speed == 0.f ? CANNON_SHOTSPEED : speed);
	GetObjectManager()->CreateObject(TYPE_BOSS_PELLET, spawnPos)->SetVelocity(Vector3f(0.f, shotSpeed, 0.f));
}

void ObjectBoss::ActivateLaser(int laserId)
{

}

void ObjectBoss::DisableLaser(int laserId)
{

}

void ObjectBoss::Draw() const
{
	GameObject::Draw();
}