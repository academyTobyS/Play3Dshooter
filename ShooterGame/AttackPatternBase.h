#pragma once
#include <vector>

class ObjectBoss;

enum eAttackPhase
{
	PHASE_A,
	PHASE_B,
	PHASE_C,
	PHASE_D,
	PHASE_E,
	PHASE_F,
	PHASE_TOTAL
};

using AttackPointer = void (*)(ObjectBoss*);
using TimedAttack = std::pair<float, AttackPointer>;

class AttackPatternBase
{
public:
	virtual void Start(ObjectBoss* pBoss) {m_loopCounter = 0; m_attackIndex = 0; m_timer = 0.f;};
	virtual void Update(ObjectBoss* pBoss);
	virtual void End(ObjectBoss* pBoss) {};
	virtual void OnLoop(ObjectBoss* pBoss) {}; // optional implementation, gets called each time pattern loops back to Attack 0
	bool PatternCanFinish() {return m_loopCounter >= m_minLoops;};

protected:
	void RegisterAttack(AttackPointer attack, float delaySincePrev = 0.f);
	int m_loopCounter{0};
	int m_minLoops{5}; // loops before CanFinish() returns true

private:
	std::vector<TimedAttack> m_vAttacks;
	float m_timer{0.f};
	int m_attackIndex;
};