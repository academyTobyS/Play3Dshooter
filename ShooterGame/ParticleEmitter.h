#pragma once
#include "Play3d.h"

struct Particle
{
	Play3d::Vector3f pos{0.f, 0.f, 0.f};
	Play3d::Vector3f velocity{ 0.f, 0.f, 0.f };
	float timeAlive{0.f};
};

struct ParticleEmitterSettings
{
	Play3d::Vector3f emitterMinExtents{-1.f, -1.f, -1.f};
	Play3d::Vector3f emitterMaxExtents{1.f, 1.f, 1.f};

	Play3d::Vector3f particleMinVelocity{0.f, 0.f, 0.f};
	Play3d::Vector3f particleMaxVelocity{0.f, 0.f, 0.f};

	Play3d::ColourValue particleColour{Play3d::Colour::White};

	float particleLifetime{10.f};
	float emitWaitMin{0.1f};
	float emitWaitMax{0.4f}; // set this to 0 if wishing to manually trigger bursts via Tick()

	int particlesPerEmit{1};
	int capacity{100};

	bool particlesRelativeToEmitter{false};
};

class ParticleEmitter
{
public:
	void ApplySettings(const ParticleEmitterSettings& rSettings);

	void Prewarm(); // slow
	void Tick(float customTime = 0.f); // may wish to trigger manually rather than every frame, hence avoiding 'update'
	void Draw() const;
	void DestroyAll();

	Play3d::Vector3f m_position{0.f, 0.f, 0.f};

private:
	std::vector<Particle> m_particles;
	ParticleEmitterSettings m_settings;
	float m_timerEmit{0.f};

	#ifdef _DEBUG
	size_t m_debugMaxParticleCount{0};
	#endif
};
