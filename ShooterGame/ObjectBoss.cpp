#include "ObjectBoss.h"
#include "ObjectManager.h"
#include "GameHud.h"

#include "ObjectBossBomb.h"
#include "AttackPatternA.h"
#include "AttackPatternB.h"

using namespace Play3d;

static constexpr float WOBBLE_STRENGTH{kfQuartPi / 4.f};
static constexpr float POS_LIMIT_X{8.f};
static constexpr float CANNON_SPACING{0.75f};
static constexpr float CANNON_SHOTSPEED{-0.05f};
static constexpr int TOTAL_CANNONS{7};
static constexpr float COOLDOWN_FIRE{3.5f};

AttackPatternA patternA;
AttackPatternB patternB;

ObjectBoss::ObjectBoss(Play3d::Vector3f position) : GameObject(TYPE_BOSS, position)
{
	// Get resources
	GameObjectManager* pObj{GetObjectManager()};
	m_meshId = pObj->GetMesh("..\\Assets\\Models\\_station.obj");
	m_materialId = pObj->GetMaterial("..\\Assets\\Models\\_station-red.jpg");
	m_sfxFirePellet[0] = pObj->GetAudioId("..\\Assets\\Audio\\BossPellet1.wav");
	m_sfxFirePellet[1] = pObj->GetAudioId("..\\Assets\\Audio\\BossPellet2.wav");
	m_sfxFirePellet[2] = pObj->GetAudioId("..\\Assets\\Audio\\BossPellet3.wav");
	m_sfxFireBomb[0] = pObj->GetAudioId("..\\Assets\\Audio\\BossBomb1.wav");

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
	RegisterAttackPattern(&patternB, eAttackPhase::PHASE_B);
	m_phase = eAttackPhase::PHASE_B;
	m_phases[m_phase]->Start(this);
}

void ObjectBoss::ActivateAttackPattern(eAttackPhase newPhase)
{
	m_phases[m_phase]->End(this);
	m_phases[newPhase]->Start(this);
	m_phase = newPhase;
}

void ObjectBoss::ToggleAutocannon(bool enabled, float interval, int groupSize, float groupDelay)
{
	if (enabled != m_autocannonActive || interval != m_autocannonInterval)
	{
		// Apply settings
		m_autocannonActive = enabled;
		m_autocannonInterval = interval;
		m_autocannonGroupsize = groupSize;
		m_autocannonGroupdelay = groupDelay;

		// Reset
		m_autocannonCounter = 0;
		m_autocannonTimer = 0.f;
	}
}

void ObjectBoss::Update()
{
	// ship wobble anim
	float elapsedTime = System::GetElapsedTime();
	//m_pos.x = sin(elapsedTime / 4) * POS_LIMIT_X;
	m_rotation.x = sin(elapsedTime * 2) * WOBBLE_STRENGTH / 2;
	m_rotation.y = cos(elapsedTime) * WOBBLE_STRENGTH;

	// Execute active attack pattern
	m_phases[m_phase]->Update(this);

	// Fire autocannon when enabled
	if (m_autocannonActive)
	{
		UpdateAutocannon();
	}

	// Process multishot requests (similar to autocannon except without endless loop)
	if (m_multishotRemaining)
	{
		UpdateMultishot();
	}
}

void ObjectBoss::UpdateAutocannon()
{
	m_autocannonTimer -= Play3d::System::GetDeltaTime();
	if (m_autocannonTimer <= 0.f)
	{
		FireAtPlayer();

		m_autocannonTimer = m_autocannonInterval;
		m_autocannonCounter++;
		if (m_autocannonCounter >= m_autocannonGroupsize)
		{
			m_autocannonCounter = 0;
			m_autocannonTimer = m_autocannonGroupdelay;
		}
	}
}

void ObjectBoss::UpdateMultishot()
{
	m_multishotTimer -= Play3d::System::GetDeltaTime();
	if (m_multishotTimer <= 0.f)
	{
		FireAtPlayer();

		m_multishotRemaining--;
		m_multishotTimer = m_multishotDelay;
	}
}

void ObjectBoss::FireAtPlayer(Play3d::Vector2f origin, float velocity)
{
	if (origin == Vector2f(0.f, 0.f))
	{
		origin = m_pos.xy();
		origin.y -= 2.5f;
	}
	velocity = (velocity == 0.f ? CANNON_SHOTSPEED : velocity);

	GameObject* pObj = GetObjectManager()->CreateObject(TYPE_BOSS_PELLET, Vector3f(origin.x, origin.y, 0.f));
	pObj->SetVelocity(normalize(pObj->GetPosition() - GetObjectManager()->GetPlayer()->GetPosition()) * velocity);

	AudioPellet();
}

void ObjectBoss::FireAtPlayerMulti(int shotTotal, float delayPerShot)
{
	m_multishotRemaining = shotTotal;
	m_multishotDelay = delayPerShot;
	m_multishotTimer = 0.f; // ensure first shot is immediate for responsiveness/timings
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

void ObjectBoss::FireBomb(float detonationTimer, int fragments, float angle, float velocity)
{
	Vector3f spawnPos{ m_pos };
	spawnPos.y -= 2.5f;

	ObjectBossBomb* pBomb = static_cast<ObjectBossBomb*>(GetObjectManager()->CreateObject(TYPE_BOSS_BOMB, spawnPos));
	pBomb->SetVelocity(Vector3f(sin(angle), cos(angle), 0.f) * (velocity == 0.f ? CANNON_SHOTSPEED : velocity));
	pBomb->SetDetonationTimer(detonationTimer);
	pBomb->SetFragments(fragments);

	AudioBomb();
}

void ObjectBoss::ActivateLaser(int laserId)
{

}

void ObjectBoss::DisableLaser(int laserId)
{

}

void ObjectBoss::AudioPellet()
{
	int sfxId = std::floor(RandValueInRange(0.f, SFX_PELLET_SLOTS));
	Audio::PlaySound(m_sfxFirePellet[sfxId]);
}

void ObjectBoss::AudioBomb()
{
	int sfxId = std::floor(RandValueInRange(0.f, SFX_BOMB_SLOTS));
	Audio::PlaySound(m_sfxFireBomb[sfxId]);
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