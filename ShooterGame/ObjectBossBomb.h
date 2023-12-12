#pragma once
#include "GameObject.h"

class ObjectBossBomb : public GameObject
{
public:
	ObjectBossBomb(Play3d::Vector3f position);

	void SetDetonationTimer(float timer){m_detonationTimer = timer;};

	void Update() override;
	//void Draw() const override;

	void OnCollision(GameObject* other) override;

private:
	void Burst();
	float m_detonationTimer{2.f};
};

