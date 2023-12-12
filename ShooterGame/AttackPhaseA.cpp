#include "AttackPhaseA.h"
#include "ObjectBoss.h"

using namespace Play3d;

AttackPhaseA::AttackPhaseA()
{
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstRadial(-kfQuartPi, kfQuartPi, 6, 0.f, pBoss->GetPosition().xy() + Vector2f(5.f, 0.f));
		pBoss->FireBurstRadial(-kfQuartPi, kfQuartPi, 6, 0.f, pBoss->GetPosition().xy() + Vector2f(-5.f, 0.f));
	},
	.8f);

	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstRadial(-kfQuartPi, kfQuartPi, 8);
	},
	.8f);
}

void AttackPhaseA::Start(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(true);
}

void AttackPhaseA::End(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(false);
}