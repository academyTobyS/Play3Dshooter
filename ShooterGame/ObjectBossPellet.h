#pragma once
#include "GameObject.h"

class ObjectBossPellet : public GameObject
{
public:
	ObjectBossPellet(Play3d::Vector3f position);

	void Update() override;
	//void Draw() const override;

private:
};

