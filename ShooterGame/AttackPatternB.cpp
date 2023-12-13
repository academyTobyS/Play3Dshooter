#include "AttackPatternB.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPatternB::AttackPatternB()
{
	m_minLoops = 2;

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireAtPlayerMulti(16, 0.1f);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, 8, kfQuartPi);
	},
	2.f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, 8, 0.f);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, 8, -kfQuartPi);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireAtPlayerMulti(16, 0.1f);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, 8, -kfQuartPi);
	},
	2.f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, 8, 0.f);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f,8,  kfQuartPi);
	},
	0.5f);
}