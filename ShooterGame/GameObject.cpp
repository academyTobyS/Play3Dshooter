#include "GameObject.h"

using namespace Play3d;

GameObject::GameObject( GameObjectType objType, Vector2f position)
{
	m_type = objType;
	m_pos = position;
}

void GameObject::SetSprite( std::string spriteName, float animationSpeed )
{
	//m_spriteID = Play::GetSpriteId( spriteName.c_str() );
	m_animSpeed = animationSpeed;
};

// Code adapted from Play::IsColliding
bool GameObject::IsColliding( GameObject* obj )
{
	//Don't collide with NULL objects
	if( m_type == -1 || obj->m_type == -1 )
		return false;

	int xDiff = (int)m_pos.x- (int)obj->m_pos.x;
	int yDiff = (int)m_pos.y - (int)obj->m_pos.y;
	int radii = (int)m_radius + (int)obj->m_radius;

	// Game programmers don't do square root!
	return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
}

void GameObject::StandardMovementUpdate()
{
	m_rotation += m_rotSpeed;
	m_oldPos = m_pos;
	m_velocity += m_acceleration;
	m_pos += m_velocity;
}

void GameObject::Destroy( )
{
	m_destroy = true;
}

void GameObject::UpdateAnimation()
{
	m_frameTimer += m_animSpeed;

	if( m_frameTimer >= 1 )
	{
		m_frame++;
		m_frameTimer = 0;
	}
}