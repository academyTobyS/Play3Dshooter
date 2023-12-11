#pragma once
#include "AttackPhase.h"

class AttackPhaseA : public AttackPattern
{
public:
	AttackPhaseA();

	void Start(ObjectBoss* pBoss) override;
	void End(ObjectBoss* pBoss) override;
};