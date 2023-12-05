#include "ObjectAsteroid.h"
#include "ObjectManager.h"
using namespace Play3d;

static constexpr float MAX_WOBBLE{kfPi};

ObjectAsteroid::ObjectAsteroid(Play3d::Vector3f position) : GameObject(TYPE_BOSS, position)
{
	m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\asteroid.obj");
	m_materialId = GetObjectManager()->GetMaterial();
}

void ObjectAsteroid::Update()
{
	float elapsedTime = System::GetElapsedTime();

	m_rotation.x = sin(elapsedTime * 0.33f) * MAX_WOBBLE;
	m_rotation.y = sin(elapsedTime * 0.5f) * MAX_WOBBLE;
	m_rotation.z = cos(elapsedTime * 0.63f) * MAX_WOBBLE;
}