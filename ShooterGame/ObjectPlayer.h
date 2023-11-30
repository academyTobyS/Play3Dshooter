#pragma once
#include "GameObject.h"

class ObjectPlayer : public GameObject
{
public:
	ObjectPlayer(Play3d::Vector3f position);

	void Update() override;
	void Draw() const override;
};

