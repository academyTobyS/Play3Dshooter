#pragma once
#include "GameObject.h"

class ObjectShipChunk : public GameObject
{
public:
	ObjectShipChunk(GameObjectType type, Play3d::Vector3f position);

	void Update() override;
	void Draw() const override;

private:
	float m_lifetime{0.f};
};