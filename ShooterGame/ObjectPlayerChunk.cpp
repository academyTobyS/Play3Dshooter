#include "ObjectPlayerChunk.h"
#include "ObjectManager.h"
using namespace Play3d;

ObjectPlayerChunk::ObjectPlayerChunk(GameObjectType type, Play3d::Vector3f position) : GameObject(type, position)
{
	switch (type)
	{
		case TYPE_PLAYER_CHUNK_CORE:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_fighter-chunk-core.obj");
			PLAY_ASSERT(m_meshId.IsValid());
			break;

		case TYPE_PLAYER_CHUNK_WING_L:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_fighter-chunk-wingL.obj");
			break;

		case TYPE_PLAYER_CHUNK_WING_R:
			m_meshId = GetObjectManager()->GetMesh("..\\Assets\\Models\\_fighter-chunk-wingR.obj");
			break;
	}
	m_materialId = GetObjectManager()->GetMaterial("..\\Assets\\Models\\_fighter-blue.jpg");
}

void ObjectPlayerChunk::Update()
{
	m_lifetime -= System::GetDeltaTime();
	if(m_lifetime <= 0.f)
	{
		Destroy();
	}
}

void ObjectPlayerChunk::Draw() const
{
	GameObject::Draw();
}