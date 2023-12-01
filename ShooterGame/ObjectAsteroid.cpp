#include "ObjectAsteroid.h"
using namespace Play3d;

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

static constexpr float MAX_WOBBLE{kfPi};

ObjectAsteroid::ObjectAsteroid(Play3d::Vector3f position) : GameObject(TYPE_BOSS, position)
{
	m_meshId = AssignMesh(s_meshId, "..\\Assets\\Models\\asteroid.obj");
	m_materialId = AssignMaterial(s_materialId);
}

void ObjectAsteroid::Update()
{
	float elapsedTime = System::GetElapsedTime();

	m_rotation.x = sin(elapsedTime * 0.33f) * MAX_WOBBLE;
	m_rotation.y = sin(elapsedTime * 0.5f) * MAX_WOBBLE;
	m_rotation.z = cos(elapsedTime * 0.63f) * MAX_WOBBLE;
}