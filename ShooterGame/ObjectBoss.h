#pragma once
#include "GameObject.h"
#include "AttackPatternBase.h"

static constexpr int BOSS_MAX_HEALTH{1000};

static constexpr int SFX_PELLET_SLOTS{3};	// 3 unique audio for firing pellet
static constexpr int SFX_BOMB_SLOTS{1};		// 2 unique audio for firing bomb

class ObjectBoss : public GameObject
{
public:
	ObjectBoss(Play3d::Vector3f position);

	// General
	void RegisterAttackPattern(AttackPatternBase* pPhase, eAttackPhase type){m_phases[type] = pPhase; };
	void ActivateAttackPattern(eAttackPhase pattern);
	void Update() override;
	void Draw() const override;
	void OnCollision(GameObject* other) override;

	// Weapon One-Shots
	void FireAtPlayer(float angleOffset = 0.f, Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f), float velocity = 0.f);
	void FireAtPlayerMulti(int shotTotal, float delayPerShot, float angleOffset = 0.f);
	void FireSingle(int spacingIncrement, float angle, float velocity = 0.f);
	void FireBurstRadial(float minAngle, float maxAngle, int segments, float velocity = 0.f, Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f));
	void FireBurstBlock(float xSpacing, int segments, float xOffset = 0.f);
	void FireBomb(float detonationTimer, int fragments = 12, float angle = 0.f, float velocity = -0.f);

	// Weapon Toggles
	void ToggleAutocannon(bool enabled, float interval = 0.2f, int groupSize = 8, float groupDelay = 1.5f);
	void ActivateLaser(int laserId);
	void DisableLaser(int laserId);

	// Randomised Audio
	void AudioPellet();
	void AudioBomb();
	void AudioDamage();

private:
	void UpdateAttackPattern();
	void UpdateAutocannon();
	void UpdateMultishot();

	// Boss Data
	AttackPatternBase* m_phases[PHASE_TOTAL];
	int m_phase{PHASE_A};
	int m_health{ BOSS_MAX_HEALTH };

	// Audio Data
	Play3d::Audio::SoundId m_sfxFirePellet[SFX_PELLET_SLOTS]; 
	Play3d::Audio::SoundId m_sfxFireBomb[SFX_BOMB_SLOTS];

	// Targeted MultiShots
	struct MultishotRequest
	{
		int pendingShots{ 0 };
		float delayPerShot{ 0.5f };
		float angleOffset{ 0.f };
		float timer{0.f};
	};
	std::vector<MultishotRequest> m_vMultishotRequests;

	// Autocannon
	float m_autocannonInterval{.5f};
	float m_autocannonTimer{0.f};
	float m_autocannonGroupdelay{0.5f};
	int m_autocannonGroupsize{6};
	int m_autocannonCounter{0};
	bool m_autocannonActive{false};
};
