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

	// Weapons
	void FireAtPlayer(Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f), float velocity = 0.f);
	void FireSingle(int spacingIncrement, float angle, float velocity = 0.f);
	void FireBurstRadial(float minAngle, float maxAngle, int segments, float velocity = 0.f, Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f));
	void FireBurstBlock(float minX, float maxX, int segments, float velocity = 0.f, Play3d::Vector2f origin = Play3d::Vector2f(0.f, 0.f));
	void FireBomb();
	void ToggleAutocannon(bool enabled, float interval = 0.5f);
	void ActivateLaser(int laserId);
	void DisableLaser(int laserId);

private:
	// Boss Data
	AttackPattern* m_phases[PHASE_TOTAL];
	eAttackPhase m_phase{PHASE_A};
	int m_health{ BOSS_MAX_HEALTH };

	// Autocannon
	float m_autocannonInterval{1.f};
	float m_autocannonTimer{0.f};
	bool m_autocannonActive{false};
};
