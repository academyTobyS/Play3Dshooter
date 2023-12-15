#include "AttackPatternE.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPatternE::AttackPatternE()
{
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(1.f, 8);
	},
	.25f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(1.f, 8, kfQuartPi);
		pBoss->FireBomb(1.f, 8, -kfQuartPi);
	},
	.25f);
}
