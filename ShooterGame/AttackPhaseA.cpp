#include "AttackPhaseA.h"
#include "ObjectBoss.h"

using namespace Play3d;

void AttackA(ObjectBoss* pBoss)
{
	pBoss->FireBurstRadial(-kfQuartPi, kfQuartPi, 8, 0.f, pBoss->GetPosition().xy() + Vector2f(5.f, 0.f) );
	pBoss->FireBurstRadial(-kfQuartPi, kfQuartPi, 8, 0.f, pBoss->GetPosition().xy() + Vector2f(-5.f, 0.f) );
}

void AttackB(ObjectBoss* pBoss)
{
	pBoss->FireBurstRadial(-kfQuartPi, kfQuartPi, 8);
}

AttackPhaseA::AttackPhaseA()
{
	RegisterAttack(AttackA, .8f);
	RegisterAttack(AttackB, .8f);
}

void AttackPhaseA::Start(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(true, 0.5f);
}

void AttackPhaseA::End(ObjectBoss* pBoss)
{
	//pBoss->ToggleAutocannon(false);
}