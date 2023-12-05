#include "ObjectPlayerChunk.h"
using namespace Play3d;

static Graphics::MeshId s_meshChunkCore{};
static Graphics::MeshId s_meshChunkWingL{};
static Graphics::MeshId s_meshChunkWingR{};
static Graphics::MaterialId s_materialId{};

ObjectPlayerChunk::ObjectPlayerChunk(GameObjectType type, Play3d::Vector3f position) : GameObject(type, position)
{
	switch (type)
	{
		case TYPE_PLAYER_CHUNK_CORE:
			m_meshId = AssignMesh(s_meshChunkCore, "..\\Assets\\Models\\_fighter-chunk-core.obj");
			PLAY_ASSERT(m_meshId.IsValid());
			break;

		case TYPE_PLAYER_CHUNK_WING_L:
			m_meshId = AssignMesh(s_meshChunkWingL, "..\\Assets\\Models\\_fighter-chunk-wingL.obj");
			break;

		case TYPE_PLAYER_CHUNK_WING_R:
			m_meshId = AssignMesh(s_meshChunkWingR, "..\\Assets\\Models\\_fighter-chunk-wingR.obj");
			break;
	}
	m_materialId = AssignMaterial(s_materialId, "..\\Assets\\Models\\_fighter-blue.jpg");
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