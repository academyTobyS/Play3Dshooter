#pragma once
#include "GameObject.h"

class ObjectPlayer : public GameObject
{
public:
	ObjectPlayer(Play3d::Vector3f position);

	void Update() override;

private:
	float m_shootCooldown{0.f};
};

