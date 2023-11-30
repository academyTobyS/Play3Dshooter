#include "ObjectPlayer.h"
using namespace Play3d;

static constexpr float FLY_SPEED{.6f};
static constexpr float MAX_SPEED{.1f};

static constexpr float SPIN_SPEED{.8f};
static constexpr float MAX_ROT_SPEED{.1f};

static constexpr float MAX_ROT{kfQuartPi / 2.f};

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

ObjectPlayer::ObjectPlayer(Vector3f position) : GameObject(TYPE_PLAYER, position)
{
	m_meshId = AssignMesh(s_meshId, "..\\Assets\\Models\\ship.obj");
	m_materialId = AssignMaterial(s_materialId);

	// custom HLSL possible, example via:
	//Play3d::Demo::CreateComplexMaterial()
}

void ObjectPlayer::Update()
{
	if (Input::IsKeyDown(VK_LEFT))
	{
		m_velocity.x = std::min(m_velocity.x + (FLY_SPEED * System::GetDeltaTime()), MAX_SPEED);

		// reduce rotation faster when coming from opposing direction
		if (m_rotSpeed.y > 0)
		{
			m_rotSpeed.y *= 0.7f;
		}
		m_rotSpeed.y = std::max(m_rotSpeed.y - (SPIN_SPEED * System::GetDeltaTime()), -MAX_ROT_SPEED);
	}
	else if (Input::IsKeyDown(VK_RIGHT))
	{
		m_velocity.x = std::max(m_velocity.x - (FLY_SPEED * System::GetDeltaTime()), -MAX_SPEED);

		// reduce rotation faster when coming from opposing direction
		if (m_rotSpeed.y < 0)
		{
			m_rotSpeed.y *= 0.7f;
		}
		m_rotSpeed.y = std::min(m_rotSpeed.y + (SPIN_SPEED * System::GetDeltaTime()), MAX_ROT_SPEED);
	}
	else
	{
		m_velocity.x *= 0.86f; // reduce speed
		m_rotSpeed.y *= 0.86;  // reduce spin
		m_rotation.y *= 0.86;  // reduce current angle
	}
	m_rotation.y = std::clamp(m_rotation.y, -MAX_ROT, MAX_ROT);
}

void ObjectPlayer::Draw() const
{
	Matrix4x4f transform = MatrixTranslate<f32>(m_pos.x, m_pos.y, m_pos.z) 
						 * MatrixRotationX<f32>(m_rotation.x) 
						 * MatrixRotationY<f32>(m_rotation.y) 
						 * MatrixRotationZ<f32>(m_rotation.z);

	Graphics::SetMaterial(m_materialId);
	Graphics::DrawMesh(m_meshId, transform);

	
}