#pragma once
#include "GameObject.h"

struct Attack
{
	int cannonId; // -1 for BOMB
	float velocity;
	Play3d::Vector3f direction;
};
using TimedAttack = std::pair<Play3d::f32, Attack>;

struct AttackPattern
{
	std::vector<TimedAttack> v_attacks;
	float timer{0.f};
	int progress{0};
};

class ObjectBoss : public GameObject
{
public:
	ObjectBoss(Play3d::Vector3f position);

	void Update() override;
	void Draw() const override;

private:
	void FireCannon(int cannonId, float speed = 0.f);
	void FireBomb();
	void ActivateLaser(int laserId);
	void DisableLaser(int laserId);
	float m_cannonCooldown{0.f};

	AttackPattern phaseA;
};

