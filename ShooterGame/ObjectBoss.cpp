#include "ObjectBoss.h"

using namespace Play3d;

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

static constexpr float WOBBLE_STRENGTH{kfQuartPi / 4.f};

ObjectBoss::ObjectBoss(Play3d::Vector3f position) : GameObject(TYPE_BOSS, position)
{
	m_meshId = AssignMesh(s_meshId, "..\\Assets\\Models\\boss.obj");
	m_materialId = AssignMaterial(s_materialId);
}

void ObjectBoss::Update()
{
	float elapsedTime = System::GetElapsedTime();

	m_rotation.x = sin(elapsedTime * 2) * WOBBLE_STRENGTH / 2;
	m_rotation.y = cos(elapsedTime) * WOBBLE_STRENGTH;
}

void ObjectBoss::Draw() const
{
	GameObject::Draw();
}