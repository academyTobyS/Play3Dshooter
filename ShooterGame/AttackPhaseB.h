#pragma once
#include "AttackPhase.h"

class AttackPhaseB : public AttackPattern
{
public:
	AttackPhaseB();

	void Start(ObjectBoss* pBoss) override;
	void End(ObjectBoss* pBoss) override;
};

