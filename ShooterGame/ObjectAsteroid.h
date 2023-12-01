#pragma once
#include "GameObject.h"

class ObjectAsteroid : public GameObject
{
public:
	ObjectAsteroid(Play3d::Vector3f position);

	void Update() override;
	//void Draw() const override;

private:
};

