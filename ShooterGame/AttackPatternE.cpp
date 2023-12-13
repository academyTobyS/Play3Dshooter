#include "AttackPatternE.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPatternE::AttackPatternE()
{
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(1.f, 14);
	},
	.25f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(1.f, 14, kfQuartPi);
		pBoss->FireBomb(1.f, 14, -kfQuartPi);
	},
	.25f);
}
