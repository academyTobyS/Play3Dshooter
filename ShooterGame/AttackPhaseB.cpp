#include "AttackPhaseB.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPhaseB::AttackPhaseB()
{
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireAtPlayerMulti(16, 0.1f);
	},
	0.f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, kfQuartPi);
	},
	2.f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, 0.f);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, -kfQuartPi);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireAtPlayerMulti(16, 0.1f);
	},
	0.f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, -kfQuartPi);
	},
	2.f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, 0.f);
	},
	0.5f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBomb(2.f, kfQuartPi);
	},
	0.5f);
}

void AttackPhaseB::Start(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(true);
}

void AttackPhaseB::End(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(false);
}