#pragma once
#include "GameObject.h"

class ObjectPlayerChunk : public GameObject
{
public:
	ObjectPlayerChunk(GameObjectType type, Play3d::Vector3f position);

	void Update() override;
	void Draw() const override;

private:
	float m_lifetime{1.f};
};