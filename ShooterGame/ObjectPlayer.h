#pragma once
#include "GameObject.h"
#include "ParticleEmitter.h"

class ObjectPlayer : public GameObject
{
public:
	ObjectPlayer(Play3d::Vector3f position);

	void Update() override;
	void Respawn();
	void HandleControls();
	void OnCollision(GameObject* other) override;

	void Draw() const override;

private:
	float m_invincibilityTimer{0.f};

	bool m_bIsAlive{true};
	bool m_bDoubleTapLeft{false};
	bool m_bDoubleTapRight{false};
	bool m_bIsBarrelRoll{false};
	float m_shootCooldown{0.f};
	float m_rollCooldown{0.f};
	float m_respawnCooldown{0.f};
	ParticleEmitter m_emitterLeftThruster;
	ParticleEmitter m_emitterRightThruster;

	int m_lives{3};
};