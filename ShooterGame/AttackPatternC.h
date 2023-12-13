#pragma once
#include "AttackPatternBase.h"

// Pattern C: Pachinko Walls
class AttackPatternC : public AttackPatternBase
{
public:
	AttackPatternC();

	void OnLoop(ObjectBoss* pBoss) override;

private:
	bool m_bDoubleWall{false};
};