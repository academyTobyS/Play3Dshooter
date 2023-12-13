#pragma once
#include "AttackPatternBase.h"

// Pattern B: Medium Difficulty - Triple Bomb -> autocannon burst -> repeat
class AttackPatternB : public AttackPatternBase
{
public:
	AttackPatternB();

	void Start(ObjectBoss* pBoss) override;
	void End(ObjectBoss* pBoss) override;
};

