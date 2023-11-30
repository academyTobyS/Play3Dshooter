#pragma once
#include "Play3D.h"

// The GameObject type is the only representation of type which is visible to code externally
enum GameObjectType
{
	TYPE_NULL = -1,
	TYPE_PLAYER
};

class GameObject
{
public:
	// Constructors and (virtual) destructor
	GameObject() = delete; // Delete the default constructor as we don't want unmanaged GameObjects
	GameObject(GameObjectType objType, Play3d::Vector2f position);
	virtual ~GameObject() {}; 
	
	// Providing no implementation of the virtual Update function makes this an abstract base class
	virtual void Update() = 0;
	virtual void Draw() = 0;
	// These virtuals have implementations and so are optional overrides
	virtual void OnCollision( GameObject* ) {};

	// Various tests on game objects
	bool IsDestroyed() { return m_destroy; }
	bool IsHidden() { return m_hidden; }
	bool IsColliding(GameObject* obj);

	// Standard updates and destruction flagging
	void StandardMovementUpdate();
	void UpdateAnimation();
	void Destroy();

	// Setters
	void SetVelocity(Play3d::Vector2f velocity) { m_velocity = velocity; }
	void SetAcceleration(Play3d::Vector2f acceleration) { m_acceleration = acceleration; }
	void SetPosition(Play3d::Vector2f pos ) { m_pos = pos; }
	void SetSprite( std::string spriteName, float animationSpeed );
	void SetAnimationSpeed( float animationSpeed ) { m_animSpeed = animationSpeed; }
	void SetRotation( float rotation ) { m_rotation = rotation; }
	void SetRotationSpeed( float rotationSpeed ) { m_rotSpeed = rotationSpeed; }
	void SetFrame( float frame ) { m_frame = frame; }
	void SetVisible( bool render ) { m_hidden = render; }

	// Getters
	GameObjectType GetObjectType() { return m_type; }
	Play3d::Vector2f GetPosition() { return m_pos; }
	Play3d::Vector2f GetVelocity() { return m_velocity; }
	Play3d::Vector2f GetAcceleration() { return m_acceleration; }
	float GetRotation() { return m_rotation; }

protected:

	// Mostly just adapted from Play::GameObject
	GameObjectType m_type{ GameObjectType::TYPE_NULL };

	Play3d::Graphics::MeshId m_meshId{ -1 };
	Play3d::Graphics::MaterialId m_materialId{ -1 };

	Play3d::Vector2f m_pos{ 0, 0 };
	Play3d::Vector2f m_oldPos{ 0, 0 };
	Play3d::Vector2f m_velocity{ 0, 0 };
	Play3d::Vector2f m_acceleration{ 0, 0 };

	float m_frame{ -1 };
	float m_frameTimer{ 0 };
	float m_animSpeed{ 1 }; 
	float m_rotation{ 0 };
	float m_rotSpeed{ 0 };
	float m_radius{ -1 };

	bool m_destroy{ false };
	bool m_hidden{ false };
};