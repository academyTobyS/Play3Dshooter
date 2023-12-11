#pragma once
#include <vector>

class ObjectBoss;

enum eAttackPhase
{
	PHASE_A,
	PHASE_B,
	PHASE_TOTAL
};

using AttackPointer = void (*)(ObjectBoss*);
using TimedAttack = std::pair<float, AttackPointer>;

class AttackPattern
{
public:
	virtual void Start(ObjectBoss* pBoss) {};
	virtual void Update(ObjectBoss* pBoss);
	virtual void End(ObjectBoss* pBoss) {};

protected:
	void RegisterAttack(AttackPointer attack, float delaySincePrev = 0.f);

private:
	std::vector<TimedAttack> m_vAttacks;
	float m_timer{0.f};
	int m_attackIndex;
};