#include "ObjectBoss.h"
#include "ObjectManager.h"
#include "GameHud.h"

#include "AttackPhaseA.h"

using namespace Play3d;

static constexpr float WOBBLE_STRENGTH{kfQuartPi / 4.f};
static constexpr float POS_LIMIT_X{8.f};
static constexpr float CANNON_SPACING{0.75f};
static constexpr float CANNON_SHOTSPEED{-0.05f};
static constexpr int TOTAL_CANNONS{7};
static constexpr float COOLDOWN_FIRE{3.5f};

AttackPhaseA patternA;

ObjectBoss::ObjectBoss(Play3d::Vector3f position) : GameObject(TYPE_BOSS, position)
{
	m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_station.obj");
	m_materialId = GetObjectManager()->GetMaterial("..\\Assets\\Models\\_station-red.jpg");

	// Gateway
	m_colliders[0].type = CollisionMode::COLL_RECT;
	m_colliders[0].extents = Vector2f(1.5f, 0.75f);
	m_colliders[0].offset.x = -3.7f;

	// Cannon
	m_colliders.push_back(CollisionData());
	m_colliders[1].type = CollisionMode::COLL_RECT;
	m_colliders[1].extents = Vector2f(.3f, 1.5f);
	m_colliders[1].offset.y = -1.f;

	// Body
	m_colliders.push_back(CollisionData());
	m_colliders[2].type = CollisionMode::COLL_RECT;
	m_colliders[2].extents = Vector2f(2.5f, 0.3f);
	m_colliders[2].offset.x = 0.f;

	// Setup attack patterns
	RegisterAttackPattern(&patternA, eAttackPhase::PHASE_A);
	m_phases[m_phase]->Start(this);
}

void ObjectBoss::ActivateAttackPattern(eAttackPhase newPhase)
{
	m_phases[m_phase]->End(this);
	m_phases[newPhase]->Start(this);
	m_phase = newPhase;
}

void ObjectBoss::ToggleAutocannon(bool enabled, float interval)
{
	if (enabled != m_autocannonActive || interval != m_autocannonInterval)
	{
		m_autocannonActive = enabled;
		m_autocannonInterval = interval;
		m_autocannonTimer = 0.f;
	}
}

void ObjectBoss::Update()
{
	// ship wobble anim
	float elapsedTime = System::GetElapsedTime();
	m_rotation.x = sin(elapsedTime * 2) * WOBBLE_STRENGTH / 2;
	m_rotation.y = cos(elapsedTime) * WOBBLE_STRENGTH;
	//m_pos.x = sin(elapsedTime / 4) * POS_LIMIT_X;

	// Execute registered attack patterns
	m_phases[m_phase]->Update(this);

	// Fire autocannon when enabled
	if (m_autocannonActive)
	{
		m_autocannonTimer += Play3d::System::GetDeltaTime();
		if (m_autocannonTimer >= m_autocannonInterval)
		{
			m_autocannonTimer = 0.f;
			FireAtPlayer();
		}
	}
}

void ObjectBoss::FireAtPlayer(Play3d::Vector2f origin, float velocity)
{
	origin = (origin == Vector2f(0.f, 0.f) ? m_pos.xy() : origin);
	velocity = (velocity == 0.f ? CANNON_SHOTSPEED : velocity);

	GameObject* pObj = GetObjectManager()->CreateObject(TYPE_BOSS_PELLET, Vector3f(origin.x, origin.y, 0.f));
	pObj->SetVelocity(normalize(pObj->GetPosition() - GetObjectManager()->GetPlayer()->GetPosition()) * velocity);
}

void ObjectBoss::FireSingle(int spacingIncrement, float angle, float velocity)
{
	Vector3f spawnPos{ m_pos };
	spawnPos.y -= 0.8f;
	spawnPos.x += 0.38f;
	spawnPos.x += CANNON_SPACING * spacingIncrement;

	// Default to CANNON_SHOTSPEED but allow custom speeds
	float shotSpeed = (velocity == 0.f ? CANNON_SHOTSPEED : velocity);
	GetObjectManager()->CreateObject(TYPE_BOSS_PELLET, spawnPos)->SetVelocity(Vector3f(0.f, shotSpeed, 0.f));
}

void ObjectBoss::FireBurstRadial(float minAngle, float maxAngle, int segments, float velocity, Play3d::Vector2f origin)
{
	origin = (origin == Vector2f(0.f, 0.f) ? m_pos.xy() : origin);
	GameObjectManager* pObjs{GetObjectManager()};
	float angleIncrement = (maxAngle - minAngle) / (segments - 1);
	for (int i = 0; i < segments; i++)
	{
		GameObject* pObj = pObjs->CreateObject(TYPE_BOSS_PELLET, Vector3f(origin.x, origin.y, 0.f));
		float theta = minAngle + (angleIncrement * i);
		pObj->SetVelocity(Vector3f(sin(theta), cos(theta), 0.f) * (velocity == 0.f ? CANNON_SHOTSPEED : velocity));
	}
}

void ObjectBoss::FireBurstBlock(float minX, float maxX, int segments, float velocity, Play3d::Vector2f origin)
{
	origin = (origin == Vector2f(0.f, 0.f) ? m_pos.xy() : origin);
	GameObjectManager* pObjs{ GetObjectManager() };
	float distrib = (maxX - minX) / (segments - 1);
	for (int i = 0; i < segments; i++)
	{
		float xOffset = minX + (distrib * i);
		GameObject* pObj = pObjs->CreateObject(TYPE_BOSS_PELLET, Vector3f(origin.x + xOffset, origin.y, 0.f));
		pObj->SetVelocity(Vector3f(0.f, (velocity == 0.f ? CANNON_SHOTSPEED : velocity), 0.f));
	}
}

void ObjectBoss::FireBomb()
{
	Vector3f spawnPos{ m_pos };
	spawnPos.y -= 1.f;

	GetObjectManager()->CreateObject(TYPE_BOSS_BOMB, spawnPos)->SetVelocity(Vector3f(0.f, CANNON_SHOTSPEED, 0.f));
}

void ObjectBoss::ActivateLaser(int laserId)
{

}

void ObjectBoss::DisableLaser(int laserId)
{

}

void ObjectBoss::OnCollision(GameObject* other)
{
	if (other->GetObjectType() == GameObjectType::TYPE_PLAYER_PELLET)
	{
		m_health--;
		GameHud::Get()->SetBossBarPercent((f32)m_health / BOSS_MAX_HEALTH);
	}
}

void ObjectBoss::Draw() const
{
	GameObject::Draw();
}