#include "AttackPatternD.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPatternD::AttackPatternD()
{
	RegisterAttack([](ObjectBoss* pBoss) -> void
		{
			// Dispersal 'wall'
			pBoss->FireAtPlayer(0.2f);
			
			pBoss->FireAtPlayer(0.6f);
			pBoss->FireAtPlayer(-0.2f);
			
			pBoss->FireAtPlayer(-0.6f);

			// 'Laser' streams
			pBoss->FireAtPlayerMulti(60, 0.f);
			pBoss->FireAtPlayerMulti(60, 0.f, kfQuartPi / 2.f);
			pBoss->FireAtPlayerMulti(60, 0.f, -kfQuartPi / 2.f);
		},
		2.f);
}
