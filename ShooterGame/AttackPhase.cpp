#include "AttackPhase.h"
#include "Play3d.h"

void AttackPattern::Update(ObjectBoss* pBoss)
{
	// Process registered attacks based on their relative timing data
	if (m_vAttacks[m_attackIndex].first < m_timer)
	{
		m_timer = 0.f;
		m_vAttacks[m_attackIndex].second(pBoss);
		m_attackIndex++;
		if (m_attackIndex >= m_vAttacks.size())
		{
			m_attackIndex = 0;
		}

		// Process any simultaneous attacks this frame
		while(m_vAttacks[m_attackIndex].first <= 0.f)
		{
			m_vAttacks[m_attackIndex].second(pBoss);
			m_attackIndex++;
			if (m_attackIndex >= m_vAttacks.size())
			{
				m_attackIndex = 0;
			}
		}
	}

	m_timer += Play3d::System::GetDeltaTime();
}

void AttackPattern::RegisterAttack(AttackPointer attack, float delaySincePrev)
{
	m_vAttacks.push_back(TimedAttack(delaySincePrev, attack));
}