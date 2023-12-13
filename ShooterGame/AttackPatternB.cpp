#include "AttackPatternB.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPatternB::AttackPatternB()
{
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireAtPlayerMulti(16, 0.1f);
	},
	0.f);

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
	0.f);

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

void AttackPatternB::Start(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(true);
}

void AttackPatternB::End(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(false);
}