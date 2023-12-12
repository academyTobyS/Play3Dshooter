#pragma once
#include "GameObject.h"
#include "AttackPhase.h"

static constexpr int BOSS_MAX_HEALTH{1000};

class ObjectBoss : public GameObject
{
public:
	ObjectBoss(Play3d::Vector3f position);

	// General
	void RegisterAttackPattern(AttackPattern* pPhase, eAttackPhase type){m_phases[type] = pPhase; };
	void ActivateAttackPattern(eAttackPhase pattern);
	void Update() override;
	void Draw() const override;
	void OnCollision(GameObject* other) override;

	// Weapon One-Shots
	void FireAtPlayer(Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f), float velocity = 0.f);
	void FireAtPlayerMulti(int shotTotal, float delayPerShot);
	void FireSingle(int spacingIncrement, float angle, float velocity = 0.f);
	void FireBurstRadial(float minAngle, float maxAngle, int segments, float velocity = 0.f, Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f));
	void FireBurstBlock(float minX, float maxX, int segments, float velocity = 0.f, Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f));
	void FireBomb(float detonationTimer, float angle = 0.f, float velocity = -0.f);

	// Weapon Toggles
	void ToggleAutocannon(bool enabled, float interval = 0.2f, int groupSize = 8, float groupDelay = 1.5f);
	void ActivateLaser(int laserId);
	void DisableLaser(int laserId);

private:
	void UpdateAutocannon();
	void UpdateMultishot();

	// Boss Data
	AttackPattern* m_phases[PHASE_TOTAL];
	eAttackPhase m_phase{PHASE_A};
	int m_health{ BOSS_MAX_HEALTH };

	// Aimed MultiShot
	int m_multishotRemaining{0};
	float m_multishotDelay{0.2f};
	float m_multishotTimer{0.f};

	// Autocannon
	float m_autocannonInterval{.5f};
	float m_autocannonTimer{0.f};
	float m_autocannonGroupdelay{0.5f};
	int m_autocannonGroupsize{6};
	int m_autocannonCounter{0};
	bool m_autocannonActive{false};
};
