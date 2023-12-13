#include "AttackPatternC.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPatternC::AttackPatternC()
{
	RegisterAttack([](ObjectBoss* pBoss) -> void
		{
			pBoss->FireBurstBlock(2.2f, 9);
		},
		.8f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
		{
			pBoss->FireBurstBlock(2.2f, 8);
		},
		.8f);
}

void AttackPatternC::OnLoop(ObjectBoss* pBoss)
{
	if ((m_loopCounter % 2) == 0)
	{
		 //Every other loop, deploy walls
		if(m_bDoubleWall)
		{
			pBoss->FireBurstBlock(1.1f, 8, -5.5f);
			pBoss->FireBurstBlock(1.1f, 8, 5.5f);
		}
		else
		{
			pBoss->FireBurstBlock(1.1f, 8);
		}

		m_bDoubleWall = !m_bDoubleWall;
	}
}