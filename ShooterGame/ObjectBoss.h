#pragma once
#include "GameObject.h"

class ObjectBoss : public GameObject
{
public:
	ObjectBoss(Play3d::Vector3f position);

	void Update() override;
	void Draw() const override;

private:
};

