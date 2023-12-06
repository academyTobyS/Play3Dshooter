#pragma once
#include "GameObject.h"

class ObjectBossBomb : public GameObject
{
public:
	ObjectBossBomb(Play3d::Vector3f position);

	void Update() override;
	//void Draw() const override;

	void OnCollision(GameObject* other) override;

private:
	void Burst();
};

