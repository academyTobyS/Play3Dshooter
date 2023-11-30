#include "ParticleEmitter.h"

// Helper functions
float RandValueInRange(const float min, const float max)
{
	PLAY_ASSERT(min <= max);
	return min + ((max - min) * (static_cast<float>(rand()) / RAND_MAX));
}

void ParticleEmitter::ApplySettings(const ParticleEmitterSettings& rSettings) 
{	
	m_settings = rSettings;
	m_particles.reserve(m_settings.capacity);
	srand(time(nullptr));
}

void ParticleEmitter::Prewarm()
{
	float fakeTime{0.16f};
	for (int i = 0; i < 100; i++)
	{
		Tick(fakeTime);
	}
}

void ParticleEmitter::Tick(float customTime)
{
	float deltaTime = (customTime == 0.f ? Play3d::System::GetDeltaTime() : customTime);

	// Emit new particles based on timing config
	m_timerEmit -= deltaTime;
	if (m_timerEmit < 0 || m_settings.emitWaitMax <= 0.f)
	{
		m_timerEmit = RandValueInRange(m_settings.emitWaitMin, m_settings.emitWaitMax);

		for (int i = 0; i < m_settings.particlesPerEmit; i++)
		{
			if (m_particles.size() >= m_settings.capacity)
			{
				break;
			}

			m_particles.push_back(Particle());
			Particle& p = m_particles.back();

			p.pos.x = (m_settings.particlesRelativeToEmitter ? 0 : m_position.x) + RandValueInRange(m_settings.emitterMinExtents.x, m_settings.emitterMaxExtents.x);
			p.pos.y = (m_settings.particlesRelativeToEmitter ? 0 : m_position.y) + RandValueInRange(m_settings.emitterMinExtents.y, m_settings.emitterMaxExtents.y);
			p.pos.z = (m_settings.particlesRelativeToEmitter ? 0 : m_position.z) + RandValueInRange(m_settings.emitterMinExtents.z, m_settings.emitterMaxExtents.z);

			p.velocity.x = RandValueInRange(m_settings.particleMinVelocity.x, m_settings.particleMaxVelocity.x);
			p.velocity.y = RandValueInRange(m_settings.particleMinVelocity.y, m_settings.particleMaxVelocity.y);
			p.velocity.z = RandValueInRange(m_settings.particleMinVelocity.z, m_settings.particleMaxVelocity.z);
		}
	}

	#ifdef _DEBUG
	m_debugMaxParticleCount = std::max(m_debugMaxParticleCount, m_particles.size());
	#endif

	// Update existing particles
	for (int i = m_particles.size() - 1; i >= 0; i--)
	{
		Particle& p{m_particles[i]};
		p.pos += (p.velocity * deltaTime);
		p.timeAlive += deltaTime;

		if (p.timeAlive > m_settings.particleLifetime)
		{
			m_particles.erase(m_particles.begin() + i);
		}
	}
}

void ParticleEmitter::Draw() const
{
	for (const Particle& p : m_particles)
	{
		Play3d::Graphics::DrawPoint((m_settings.particlesRelativeToEmitter ? m_position : Play3d::Vector3f(0.f, 0.f, 0.f)) + p.pos, m_settings.particleColour);
	}
}

void ParticleEmitter::DestroyAll()
{
	m_particles.clear();
}