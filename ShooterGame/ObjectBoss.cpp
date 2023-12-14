#include "ObjectBoss.h"
#include "ObjectManager.h"
#include "GameHud.h"

#include "ObjectBossBomb.h"
#include "AttackPatternA.h"
#include "AttackPatternB.h"
#include "AttackPatternC.h"
#include "AttackPatternD.h"
#include "AttackPatternE.h"
#include "AttackPatternF.h"

using namespace Play3d;

static constexpr float WOBBLE_STRENGTH{kfQuartPi / 4.f};
static constexpr float POS_LIMIT_X{8.f};
static constexpr float CANNON_SPACING{0.75f};
static constexpr float CANNON_SHOTSPEED{-0.05f};
static constexpr int TOTAL_CANNONS{7};
static constexpr float COOLDOWN_FIRE{3.5f};

static constexpr int SFX_LIMIT_PELLETS{5}; // max of 5 simultaneous audio signals per frame

AttackPatternA patternRadialBursts;
AttackPatternB patternTripleBomb;
AttackPatternC patternPachinko;
AttackPatternD patternTripleLaser;
AttackPatternE patternBombShower;
AttackPatternF patternBlockDivider;

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
	m_sfxDamage[0] = pObj->GetAudioId("..\\Assets\\Audio\\Damage1.wav");
	m_sfxDamage[1] = pObj->GetAudioId("..\\Assets\\Audio\\Damage2.wav");
	m_sfxDamage[2] = pObj->GetAudioId("..\\Assets\\Audio\\Damage3.wav");
	m_vMultishotRequests.reserve(4);

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
	RegisterAttackPattern(&patternRadialBursts, eAttackPhase::PHASE_A);
	RegisterAttackPattern(&patternTripleBomb, eAttackPhase::PHASE_B);
	RegisterAttackPattern(&patternBombShower, eAttackPhase::PHASE_C);
	RegisterAttackPattern(&patternPachinko, eAttackPhase::PHASE_D);
	RegisterAttackPattern(&patternTripleLaser, eAttackPhase::PHASE_E);
	RegisterAttackPattern(&patternBlockDivider, eAttackPhase::PHASE_F);
	m_phase = eAttackPhase::PHASE_A;
	m_phases[m_phase]->Start(this);

	// Ensure boss chunks are preloaded to avoid lag on first death
	pObj->GetMesh("..\\Assets\\Models\\_station-chunk-core.obj");
	pObj->GetMesh("..\\Assets\\Models\\_station-chunk-left.obj");
	pObj->GetMesh("..\\Assets\\Models\\_station-chunk-right.obj");
	pObj->GetMesh("..\\Assets\\Models\\_station-chunk-lower.obj");
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
	if(IsAlive())
	{
		// reset audio counter
		m_sfxPelletsThisFrame = 0;

		// ship wobble anim
		float elapsedTime = System::GetElapsedTime();
		//m_pos.x = sin(elapsedTime / 4) * POS_LIMIT_X;
		m_rotation.x = sin(elapsedTime * 2) * WOBBLE_STRENGTH / 2;
		m_rotation.y = cos(elapsedTime) * WOBBLE_STRENGTH;

		UpdateAttackPattern();

		// Process multishot requests (similar to autocannon except without endless loop)
		UpdateMultishot();
	
		// Fire autocannon whenever active
		if (m_autocannonActive)
		{
			UpdateAutocannon();
		}
	}
}

void ObjectBoss::UpdateAttackPattern()
{
	// Execute active attack pattern
	m_phases[m_phase]->Update(this);

	if (m_phases[m_phase]->PatternCanFinish())
	{
		int nextPattern = m_phase + 1;
		if (nextPattern >= eAttackPhase::PHASE_TOTAL)
		{
			nextPattern = eAttackPhase::PHASE_A;
		}

		ActivateAttackPattern(static_cast<eAttackPhase>(nextPattern));
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
	for (int i = m_vMultishotRequests.size() - 1; i >= 0; i--)
	{
		MultishotRequest& r{m_vMultishotRequests[i]};

		r.timer -= Play3d::System::GetDeltaTime();
		if (r.timer <= 0.f)
		{
			FireAtPlayer(r.angleOffset);
			r.timer = r.delayPerShot;
			r.pendingShots--;

			if (r.pendingShots <= 0)
			{
				m_vMultishotRequests.erase(m_vMultishotRequests.begin() + i);
			}
		}
	}
}

void ObjectBoss::FirePellet(Play3d::Vector2f origin, float angle, float velocity)
{
	// if no specific origin requested, make origin == ship cannon
	if (origin == Vector2f(0.f, 0.f)) 
	{
		origin = m_pos.xy();
		origin.y -= 2.5f;
	}

	// if no specific velocity requested, make velocity == default
	if (velocity == 0.f)
	{
		velocity = CANNON_SHOTSPEED;
	}

	GameObject* pObj = GetObjectManager()->CreateObject(TYPE_BOSS_PELLET, Vector3f(origin.x, origin.y, 0.f));
	pObj->SetVelocity(Vector3f(sin(angle), cos(angle), 0.f) * velocity);
	AudioPellet();
}

void ObjectBoss::FireAtPlayer(float angleOffset, Play3d::Vector2f origin, float velocity)
{
	Vector2f vecToPlayer = normalize(origin - GetObjectManager()->GetPlayer()->GetPosition().xy());
	float angle = atan2(vecToPlayer.x, vecToPlayer.y) + angleOffset;
	FirePellet(origin, angle, velocity);
}

void ObjectBoss::FireAtPlayerMulti(int shotTotal, float delayPerShot, float angleOffset)
{
	m_vMultishotRequests.push_back(MultishotRequest());
	MultishotRequest& r = m_vMultishotRequests.back();

	r.pendingShots = shotTotal;
	r.delayPerShot = delayPerShot;
	r.angleOffset = angleOffset;
}

void ObjectBoss::FireBurstRadial(float minAngle, float maxAngle, int segments, float velocity, Play3d::Vector2f origin)
{
	GameObjectManager* pObjs{GetObjectManager()};
	float angleIncrement = (maxAngle - minAngle) / (segments - 1);
	for (int i = 0; i < segments; i++)
	{
		FirePellet(origin, minAngle + (angleIncrement * i), velocity);
	}
}

void ObjectBoss::FireBurstBlock(float xSpacing, int segments, float xOffset)
{
	Play3d::Vector2f origin = m_pos.xy();
	origin.x -= (xSpacing * ((f32)(segments - 1) / 2));
	origin.x += xOffset;
	GameObjectManager* pObjs{ GetObjectManager() };
	for (int i = 0; i < segments; i++)
	{
		FirePellet(origin);
		origin.x += xSpacing;
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

void ObjectBoss::AudioPellet()
{
	if(m_sfxPelletsThisFrame < SFX_LIMIT_PELLETS)
	{
		int sfxId = std::floor(RandValueInRange(0.f, SFX_PELLET_SLOTS));
		Audio::PlaySound(m_sfxFirePellet[sfxId]);
		m_sfxPelletsThisFrame++;
	}
}

void ObjectBoss::AudioBomb()
{
	int sfxId = std::floor(RandValueInRange(0.f, SFX_BOMB_SLOTS));
	Audio::PlaySound(m_sfxFireBomb[sfxId]);
}

void ObjectBoss::AudioDamage()
{
	int sfxId = std::floor(RandValueInRange(0.f, SFX_DAMAGE_SLOTS));
	Audio::PlaySound(m_sfxDamage[sfxId], 0.25f);
}

void ObjectBoss::OnCollision(GameObject* other)
{
	if (other->GetObjectType() == GameObjectType::TYPE_PLAYER_PELLET)
	{
		AudioDamage();
		GameHud::Get()->SetBossBarPercent((f32)(m_health - 1) / BOSS_MAX_HEALTH);

		if (m_health > 0)
		{
			m_health--;
			if(m_health == 0)
			{
				Die();
			}
		}
	}
}

void ObjectBoss::Die()
{
	GameObjectManager* pObjs{ GetObjectManager() };
	GameObject* pChunk;

	pChunk = pObjs->CreateObject(TYPE_BOSS_CHUNK_CORE, m_pos);
	pChunk->SetVelocity(Vector3f(0.f, 0.01f, 0.f));
	pChunk->SetRotationSpeed(m_rotation / 8.f);

	pChunk = pObjs->CreateObject(TYPE_BOSS_CHUNK_LEFT, m_pos);
	pChunk->SetVelocity(Vector3f(0.03f, 0.f, 0.f));
	pChunk->SetRotationSpeed(Vector3f(-0.03f, 0.f, 0.f));

	pChunk = pObjs->CreateObject(TYPE_BOSS_CHUNK_RIGHT, m_pos);
	pChunk->SetVelocity(Vector3f(-0.03f, 0.f, 0.f));
	pChunk->SetRotationSpeed(Vector3f(0.03f, 0.f, 0.f));

	pChunk = pObjs->CreateObject(TYPE_BOSS_CHUNK_LOWER, m_pos);
	pChunk->SetVelocity(Vector3f(0.f, -0.02f, 0.f));
	pChunk->SetRotationSpeed(-m_rotation / 8.f);

	SetHidden(true);
	Destroy();
}

void ObjectBoss::Draw() const
{
	GameObject::Draw();
}