#include "ObjectPlayer.h"
#include "ObjectManager.h"
using namespace Play3d;

static constexpr float SHIP_HALFWIDTH{0.15f};

static constexpr float COOLDOWN_DOUBLE_TAP{0.2f};
static constexpr float BARREL_ROLL_TIME{0.25f};

static constexpr float DELAY_AUTOFIRE{0.05f};
static constexpr float DELAY_ROLL{1.f};

static constexpr float MAX_SPEED{.1f};
static constexpr float STEER_SPEED_X{.6f};
static constexpr float STEER_SPEED_Y{.5f};
static constexpr float BARREL_ROLL_SPEED{MAX_SPEED * 3};

static constexpr float SPIN_SPEED{1.f};
static constexpr float MAX_ROT_SPEED{.1f};
static constexpr float MAX_ROT_X{kfQuartPi / 2.f};
static constexpr float MAX_ROT_Y{kfQuartPi / 4.f};

static const Vector2f MIN_POS{-7.f, -1.25f};
static const Vector2f MAX_POS{7.f, 5.f};

static Graphics::MeshId s_meshId{};
static Graphics::MaterialId s_materialId{};

ObjectPlayer::ObjectPlayer(Vector3f position) : GameObject(TYPE_PLAYER, position)
{
	m_meshId = AssignMesh(s_meshId, "..\\Assets\\Models\\ship.obj");
	m_materialId = AssignMaterial(s_materialId);

	ParticleEmitterSettings s;
	s.capacity = 100;
	s.emitterMinExtents = Vector3f(-0.05f, 0.f, -0.f);
	s.emitterMaxExtents = Vector3f(0.05f, 0.f, 0.f);
	s.particleMinVelocity = Vector3f(-0.f, -8.f, 0.f);
	s.particleMaxVelocity = Vector3f(0.f, -0.5f, 0.f);
	s.emitWaitMin = 0.01f;
	s.emitWaitMax = 0.01f;
	s.particlesRelativeToEmitter = false;
	s.particleLifetime = .05f;
	s.particlesPerEmit = 8;
	s.particleColour = Colour::Orange;

	m_emitterLeftThruster.ApplySettings(s);
	m_emitterRightThruster.ApplySettings(s);
}

void ObjectPlayer::Update()
{
	float deltaTime = System::GetDeltaTime();

	// FIRE
	m_shootCooldown -= deltaTime;
	if (Input::IsKeyDown(VK_SPACE))
	{
		if (m_shootCooldown < 0)
		{
			m_shootCooldown = DELAY_AUTOFIRE;

			GameObject* pPellet = GetObjectManager()->CreateObject(TYPE_PLAYER_PELLET, m_pos + Vector3f(0.f, .33f, 0.f));
			pPellet->SetVelocity(Vector3f(0.f, .2f, 0.f));
		}
	}
	else
	{
		m_shootCooldown = 0.f; // allow rapid-fire when spamming shoot button
	}

	// STEER - VERTICAL
	if (Input::IsKeyDown('W'))
	{
		m_velocity.y = std::min(m_velocity.y + (STEER_SPEED_Y * deltaTime), MAX_SPEED);

		if (m_rotSpeed.x < 0)
		{
			m_rotSpeed.x *= 0.7f;
		}
		m_rotSpeed.x = std::min(m_rotSpeed.x + (SPIN_SPEED * deltaTime), MAX_ROT_SPEED);
	}
	else if (Input::IsKeyDown('S'))
	{
		m_velocity.y = std::max(m_velocity.y - (STEER_SPEED_Y * deltaTime), -MAX_SPEED);

		if (m_rotSpeed.x > 0)
		{
			m_rotSpeed.x *= 0.7f;
		}
		m_rotSpeed.x = std::max(m_rotSpeed.x - (SPIN_SPEED * deltaTime), -MAX_ROT_SPEED);
	}
	else
	{
		m_rotSpeed.x *= 0.86f;	// reduce spin
		m_rotation.x *= 0.86f;	// reduce angle
		m_velocity.y *= 0.9f;	// reduce speed
	}

	// STEER - HORIZONTAL
	if (Input::IsKeyDown('A'))
	{
		float thrust = std::min(m_velocity.x + (STEER_SPEED_X * deltaTime), MAX_SPEED);
		m_velocity.x = std::max(m_velocity.x, thrust); // don't clamp velocity if already above max-speed (barrel rolls)

		// reduce rotation faster when coming from opposing direction
		if (m_rotSpeed.y > 0)
		{
			m_rotSpeed.y *= 0.7f;
		}
		m_rotSpeed.y = std::max(m_rotSpeed.y - (SPIN_SPEED * deltaTime), -MAX_ROT_SPEED);
	}
	else if (Input::IsKeyDown('D'))
	{
		float thrust = std::max(m_velocity.x - (STEER_SPEED_X * deltaTime), -MAX_SPEED);
		m_velocity.x = std::min(m_velocity.x, thrust); // don't clamp velocity if already above max-speed (barrel rolls)

		// reduce rotation faster when coming from opposing direction
		if (m_rotSpeed.y < 0)
		{
			m_rotSpeed.y *= 0.7f;
		}
		m_rotSpeed.y = std::min(m_rotSpeed.y + (SPIN_SPEED * deltaTime), MAX_ROT_SPEED);
	}
	else if(!m_bIsBarrelRoll)
	{
		// Reduce all forces when not actively steering/rolling
		m_rotSpeed.y *= 0.86f; // reduce spin
		m_rotation.y *= 0.86f; // reduce current angle
		m_velocity.x *= 0.86f; // reduce speed
	}
	m_rotation.y = std::clamp(m_rotation.y, -MAX_ROT_X, MAX_ROT_X);
	m_rotation.x = std::clamp(m_rotation.x, -MAX_ROT_Y, MAX_ROT_Y);

	// BARREL ROLL - Cooldown
	m_rollCooldown -= deltaTime;
	if (m_rollCooldown <= 0.f)
	{
		m_bDoubleTapLeft = false;
		m_bDoubleTapRight = false;
		m_bIsBarrelRoll = false;
	}

	// BARREL ROLL - Execute
	if (m_bIsBarrelRoll)
	{
		float rollCompletion = 1 - (m_rollCooldown / BARREL_ROLL_TIME); // 0.f to 1.f: start to finish
		m_rotation.y = (m_bDoubleTapLeft ? -kfTwoPi : kfTwoPi) * rollCompletion;
	}
	if (!m_bIsBarrelRoll)
	{
		// Barrel rolls grant velocity greater than max speed, slowly align to max_speed after exiting rolls
		if (abs(m_velocity.x) > MAX_SPEED)
		{
			m_velocity.x *= 0.97f;
		}
	}

	// BARREL ROLL - Trigger
	if (!m_bIsBarrelRoll && Input::IsKeyPressed('A'))
	{
		if (m_bDoubleTapLeft && m_rollCooldown > 0.f)
		{
			// Double tap: Initiate Roll!
			m_velocity.x = BARREL_ROLL_SPEED;
			m_rotation.y = MAX_ROT_X;
			m_bIsBarrelRoll = true;
			m_rollCooldown = BARREL_ROLL_TIME;
		}
		else
		{
			// First press: Wait for double tap
			m_bDoubleTapLeft = true;
			m_bDoubleTapRight = false;
			m_rollCooldown = COOLDOWN_DOUBLE_TAP;
		}
	}
	else if (!m_bIsBarrelRoll && Input::IsKeyPressed('D'))
	{
		if (m_bDoubleTapRight && m_rollCooldown > 0.f)
		{
			// Double tap: Initiate Roll!
			m_velocity.x = -BARREL_ROLL_SPEED;
			m_rotation.y = -MAX_ROT_X;
			m_bIsBarrelRoll = true;
			m_rollCooldown = BARREL_ROLL_TIME;
		}
		else
		{
			// First press: Wait for double tap
			m_bDoubleTapLeft = false;
			m_bDoubleTapRight = true;
			m_rollCooldown = COOLDOWN_DOUBLE_TAP;
		}
	}

	// Thruster particle rotation about ship
	float c = cos(-m_rotation.y);
	float s = sin(-m_rotation.y);
	Vector3f thrusterLeftOffset{SHIP_HALFWIDTH, -SHIP_HALFWIDTH * 2, 0.f};
	Vector3f thrusterRightOffset{-SHIP_HALFWIDTH, -SHIP_HALFWIDTH * 2, 0.f };
	Vector3f temp = thrusterLeftOffset;
	thrusterLeftOffset.x = (temp.x * c) - (temp.z * s);
	thrusterLeftOffset.z = (temp.z * c) + (temp.x * s);
	temp = thrusterRightOffset;
	thrusterRightOffset.x = (temp.x * c) - (temp.z * s);
	thrusterRightOffset.z = (temp.z * c) + (temp.x * s);
	m_emitterLeftThruster.m_position = m_pos + thrusterLeftOffset;
	m_emitterLeftThruster.Tick();
	m_emitterRightThruster.m_position = m_pos + thrusterRightOffset;
	m_emitterRightThruster.Tick();

	// Enforce limits
	Vector3f cachedPos = m_pos;
	m_pos.x = std::clamp(m_pos.x, MIN_POS.x, MAX_POS.x);
	m_pos.y = std::clamp(m_pos.y, MIN_POS.y, MAX_POS.y);
	if (cachedPos.x != m_pos.x)
	{
		if(!m_bIsBarrelRoll)
		{
			m_velocity.x = 0.f;
		}
	}
	if (cachedPos.y != m_pos.y)
	{
		m_velocity.y = 0.f;
	}
}

void ObjectPlayer::Draw() const
{
	GameObject::Draw();
	m_emitterLeftThruster.Draw();
	m_emitterRightThruster.Draw();
}