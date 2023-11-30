#include "ObjectPlayer.h"
#include "ObjectManager.h"
using namespace Play3d;

static constexpr float AUTOSHOOT_DELAY{0.2f};

static constexpr float FLY_SPEED{.6f};
static constexpr float MAX_SPEED{.1f};

static constexpr float SPIN_SPEED{1.f};
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
	float deltaTime = System::GetDeltaTime();

	m_shootCooldown -= deltaTime;
	if (Input::IsKeyDown(VK_SPACE))
	{
		if (m_shootCooldown < 0)
		{
			m_shootCooldown = AUTOSHOOT_DELAY;

			GameObject* pPellet = GetObjectManager()->CreateObject(TYPE_PLAYER_PELLET, m_pos + Vector3f(m_velocity.x / 2, .33f, 0.f));
			pPellet->SetVelocity(Vector3f(0.f, .2f, 0.f));
		}
	}
	else
	{
		m_shootCooldown = 0.f; // allow rapid-fire when spamming shoot button
	}

	if (Input::IsKeyDown(VK_LEFT))
	{
		m_velocity.x = std::min(m_velocity.x + (FLY_SPEED * deltaTime), MAX_SPEED);

		// reduce rotation faster when coming from opposing direction
		if (m_rotSpeed.y > 0)
		{
			m_rotSpeed.y *= 0.7f;
		}
		m_rotSpeed.y = std::max(m_rotSpeed.y - (SPIN_SPEED * deltaTime), -MAX_ROT_SPEED);
	}
	else if (Input::IsKeyDown(VK_RIGHT))
	{
		m_velocity.x = std::max(m_velocity.x - (FLY_SPEED * deltaTime), -MAX_SPEED);

		// reduce rotation faster when coming from opposing direction
		if (m_rotSpeed.y < 0)
		{
			m_rotSpeed.y *= 0.7f;
		}
		m_rotSpeed.y = std::min(m_rotSpeed.y + (SPIN_SPEED * deltaTime), MAX_ROT_SPEED);
	}
	else
	{
		m_velocity.x *= 0.86f; // reduce speed
		m_rotSpeed.y *= 0.86;  // reduce spin
		m_rotation.y *= 0.86;  // reduce current angle
	}
	m_rotation.y = std::clamp(m_rotation.y, -MAX_ROT, MAX_ROT);
}