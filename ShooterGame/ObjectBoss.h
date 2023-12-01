#pragma once
#include "GameObject.h"

class ObjectBoss : public GameObject
{
public:
	ObjectBoss(Play3d::Vector3f position);

	void Update() override;
	void Draw() const override;

private:
	void FireCannon(int cannonId, float speed = 0.f);
	void ActivateLaser(int laserId);
	void DisableLaser(int laserId);
	float m_fireCooldown{0.f};
};

