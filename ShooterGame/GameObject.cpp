#include "GameObject.h"
#include "ObjectManager.h"
#include <fstream>

using namespace Play3d;

GameObject::GameObject( GameObjectType objType, Vector3f position)
{
	m_type = objType;
	m_pos = position;
	m_colliders.push_back(CollisionData());
}

void GameObject::SetSprite( std::string spriteName, float animationSpeed )
{
	//m_spriteID = Play::GetSpriteId( spriteName.c_str() );
	m_animSpeed = animationSpeed;
};

// Code adapted from Play::IsColliding
bool GameObject::IsColliding( GameObject* obj )
{
	//Don't collide with null/non-collide objects
	if( m_type == -1 || obj->m_type == -1 || !m_canCollide || !obj->m_canCollide)
		return false;

	// Sphere v Sphere
	if(m_colliders[0].type == CollisionMode::COLL_RADIAL && obj->m_colliders[0].type == CollisionMode::COLL_RADIAL)
	{
		float xDiff = (m_pos.x + m_colliders[0].offset.x) - (obj->m_pos.x + obj->m_colliders[0].offset.x);
		float yDiff = (m_pos.y + m_colliders[0].offset.y) - (obj->m_pos.y + obj->m_colliders[0].offset.y);
		float radii = m_colliders[0].radius + obj->m_colliders[0].radius;

		return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
	}

	// Sphere v Rect
	else if ((m_colliders[0].type == CollisionMode::COLL_RADIAL && obj->m_colliders[0].type == CollisionMode::COLL_RECT)
	|| (m_colliders[0].type == CollisionMode::COLL_RECT && obj->m_colliders[0].type == CollisionMode::COLL_RADIAL))
	{
		GameObject* objRadial{ (m_colliders[0].type == CollisionMode::COLL_RADIAL ? this : obj)};
		GameObject* objRect{ (m_colliders[0].type == CollisionMode::COLL_RECT ? this : obj) };

		Vector3f objRadialPos{objRadial->m_pos + Vector3f(objRadial->m_colliders[0].offset.x, objRadial->m_colliders[0].offset.y, 0.f)};
		Vector3f objRectPos{objRect->m_pos + Vector3f(objRect->m_colliders[0].offset.x, objRect->m_colliders[0].offset.y, 0.f)};

		Vector3f vecToRect{objRectPos - objRadialPos};
		float distanceToRect = length(vecToRect);

		// If distance from RadialCollider to RectCollider is smaller than collision radius, collision must be true
		if(distanceToRect < objRadial->m_colliders[0].radius)
		{
			return true;
		}
		else // Else find point on radius-perimeter nearest to rect and test if that point is within rect
		{
			Vector3f testPoint = objRadialPos + ((vecToRect / distanceToRect) * objRadial->m_colliders[0].radius);
			return (testPoint.x < objRectPos.x + objRect->m_colliders[0].extents.x 
				 && testPoint.x > objRectPos.x - objRect->m_colliders[0].extents.x
				 && testPoint.y < objRectPos.y + objRect->m_colliders[0].extents.y
				 && testPoint.y > objRectPos.y - objRect->m_colliders[0].extents.y);
		}
	}

	// Rect v Rect
	else if (m_colliders[0].type == CollisionMode::COLL_RECT && obj->m_colliders[0].type == CollisionMode::COLL_RECT)
	{
		PLAY_ASSERT_MSG(false, "Collision scenario not implemented!");
	}
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

	if(m_colliders[0].type == CollisionMode::COLL_RADIAL)
	{
		Graphics::SetMaterial(collMat);
		Graphics::DrawMesh(collMesh, MatrixTranslate<f32>(m_pos.x + m_colliders[0].offset.x, m_pos.y + m_colliders[0].offset.y, m_pos.z)
								   * MatrixScale<f32>(m_colliders[0].radius, m_colliders[0].radius, m_colliders[0].radius));
	}
	else if (m_colliders[0].type == CollisionMode::COLL_RECT)
	{
		Vector3f origin{m_pos + Vector3f(m_colliders[0].offset.x, m_colliders[0].offset.y, 0.f)};
		Graphics::DrawQuad(
			origin + Vector3f(-m_colliders[0].extents.x, m_colliders[0].extents.y, 0.f),
			origin + Vector3f(-m_colliders[0].extents.x, -m_colliders[0].extents.y, 0.f),
			origin + Vector3f(m_colliders[0].extents.x, m_colliders[0].extents.y, 0.f),
			origin + Vector3f(m_colliders[0].extents.x, -m_colliders[0].extents.y, 0.f),
			Colour::Blue
		);
	}
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