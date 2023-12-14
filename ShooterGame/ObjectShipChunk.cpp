#include "ObjectShipChunk.h"
#include "ObjectManager.h"
using namespace Play3d;

ObjectShipChunk::ObjectShipChunk(GameObjectType type, Play3d::Vector3f position) : GameObject(type, position)
{
	// Switch - MESH
	switch (type)
	{
		// Player ship chunks
		case TYPE_PLAYER_CHUNK_CORE:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_fighter-chunk-core.obj");
			break;
		case TYPE_PLAYER_CHUNK_WING_L:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_fighter-chunk-wingL.obj");
			break;
		case TYPE_PLAYER_CHUNK_WING_R:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_fighter-chunk-wingR.obj");
			break;

		// Boss ship chunks
		case TYPE_BOSS_CHUNK_CORE:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_station-chunk-core.obj");
			break;
		case TYPE_BOSS_CHUNK_LEFT:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_station-chunk-left.obj");
			break;
		case TYPE_BOSS_CHUNK_RIGHT:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_station-chunk-right.obj");
			break;
		case TYPE_BOSS_CHUNK_LOWER:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_station-chunk-lower.obj");
			break;
	}

	// Switch - GENERAL
	switch (type)
	{
	case TYPE_PLAYER_CHUNK_CORE:
	case TYPE_PLAYER_CHUNK_WING_L:
	case TYPE_PLAYER_CHUNK_WING_R:
		m_materialId = GetObjectManager()->GetMaterial("..\\Assets\\Models\\_fighter-blue.jpg");
		m_lifetime = 1.f;
		break;

	case TYPE_BOSS_CHUNK_CORE:
	case TYPE_BOSS_CHUNK_LEFT:
	case TYPE_BOSS_CHUNK_RIGHT:
	case TYPE_BOSS_CHUNK_LOWER:
		m_materialId = GetObjectManager()->GetMaterial("..\\Assets\\Models\\_station-red.jpg");
		m_lifetime = 5.f;
		break;
	}
}

void ObjectShipChunk::Update()
{
	m_lifetime -= System::GetDeltaTime();
	if(m_lifetime <= 0.f)
	{
		Destroy();
	}
}

void ObjectShipChunk::Draw() const
{
	GameObject::Draw();
}