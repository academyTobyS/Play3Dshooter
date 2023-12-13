#pragma once
#include "AttackPatternBase.h"

// Pattern A: Alternating Radial Bursts
class AttackPatternA : public AttackPatternBase
{
public:
	AttackPatternA();

	void Start(ObjectBoss* pBoss) override;
	void End(ObjectBoss* pBoss) override;
};