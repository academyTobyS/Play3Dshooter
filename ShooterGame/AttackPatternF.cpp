#include "AttackPatternF.h"
#include "ObjectBoss.h"

using namespace Play3d;

static constexpr float BLOCK_OFFSET{2.5f};
static constexpr float BLOCK_WIDTH{30};

AttackPatternF::AttackPatternF()
{
	m_minLoops = 3;

	// LEFT BLOCK
	const float offset{5.f};
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstBlock(.5f, BLOCK_WIDTH, BLOCK_OFFSET);
	},
	1.5f);
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstBlock(.5f, BLOCK_WIDTH, BLOCK_OFFSET);
	},
	.33f);
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstBlock(.5f, BLOCK_WIDTH, BLOCK_OFFSET);
	},
	.33f);

	// RIGHT BLOCK
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstBlock(.5f, BLOCK_WIDTH, -BLOCK_OFFSET);
	},
	1.5f);
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstBlock(.5f, BLOCK_WIDTH, -BLOCK_OFFSET);
	},
	.33f);
	RegisterAttack([](ObjectBoss* pBoss) -> void
	{
		pBoss->FireBurstBlock(.5f, BLOCK_WIDTH, -BLOCK_OFFSET);
	},
	.33f);
}
