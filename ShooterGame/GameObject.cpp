#include "GameObject.h"
#include "ObjectManager.h"
#include <fstream>

using namespace Play3d;

GameObject::GameObject( GameObjectType objType, Vector3f position)
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
	if( m_type == -1 || obj->m_type == -1 || !m_canCollide || !obj->m_canCollide)
		return false;

	float xDiff = (m_pos.x + m_collisionOffset.x) - (obj->m_pos.x + obj->m_collisionOffset.x);
	float yDiff = (m_pos.y + m_collisionOffset.y) - (obj->m_pos.y + obj->m_collisionOffset.y);
	float radii = m_collisionRadius + obj->m_collisionRadius;

	// Game programmers don't do square root!
	return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
}

void GameObject::Draw() const
{
	Matrix4x4f transform = 
		  MatrixTranslate<f32>(m_pos.x, m_pos.y, m_pos.z)
		* MatrixRotationX<f32>(m_rotation.x)
		* MatrixRotationY<f32>(m_rotation.y)
		* MatrixRotationZ<f32>(m_rotation.z);

	Graphics::SetMaterial(m_materialId);
	Graphics::DrawMesh(m_meshId, transform);
}

void GameObject::DrawCollision() const
{
	static Graphics::MeshId collMesh = Graphics::CreateSphere(1.f, 6, 6, Colour::Blue);
	static Graphics::MaterialId collMat = GetObjectManager()->GetMaterial();

	Graphics::SetMaterial(collMat);
	Graphics::DrawMesh(collMesh, MatrixTranslate<f32>(m_pos.x + m_collisionOffset.x, m_pos.y + m_collisionOffset.y, m_pos.z + m_collisionOffset.z) 
							   * MatrixScale<f32>(m_collisionRadius, m_collisionRadius, m_collisionRadius));
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