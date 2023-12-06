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

bool GameObject::IsColliding( GameObject* obj )
{
	//Don't test between null/non-collide objects
	if( m_type == -1 || obj->m_type == -1 || !m_canCollide || !obj->m_canCollide)
		return false;

	for (int iThis = 0; iThis < m_colliders.size(); iThis++)
	{
		for (int iOther = 0; iOther < obj->m_colliders.size(); iOther++)
		{
			// Sphere v Sphere
			if(m_colliders[iThis].type == CollisionMode::COLL_RADIAL && obj->m_colliders[iOther].type == CollisionMode::COLL_RADIAL)
			{
				float xDiff = (m_pos.x + m_colliders[iThis].offset.x) - (obj->m_pos.x + obj->m_colliders[iOther].offset.x);
				float yDiff = (m_pos.y + m_colliders[iThis].offset.y) - (obj->m_pos.y + obj->m_colliders[iOther].offset.y);
				float radii = m_colliders[iThis].radius + obj->m_colliders[iOther].radius;

				if ((xDiff * xDiff) + (yDiff * yDiff) < radii * radii)
				{
					return true;
				}
			}

			// Sphere v Rect
			else if ((m_colliders[iThis].type == CollisionMode::COLL_RADIAL && obj->m_colliders[iOther].type == CollisionMode::COLL_RECT)
				 || ( m_colliders[iThis].type == CollisionMode::COLL_RECT && obj->m_colliders[iOther].type == CollisionMode::COLL_RADIAL))
			{
				// Arrange necessary data
				GameObject* objRadial{nullptr};
				GameObject* objRect{nullptr};
				int iRad, iRect;
				if (m_colliders[iThis].type == CollisionMode::COLL_RADIAL)
				{
					objRadial = this;
					objRect = obj;
					iRad = iThis;
					iRect = iOther;
				}
				else
				{
					objRadial = obj;
					objRect = this;
					iRad = iOther;
					iRect = iThis;
				}

				// Cache values for calculation
				Vector3f objRadialPos{objRadial->m_pos + Vector3f(objRadial->m_colliders[iRad].offset.x, objRadial->m_colliders[iRad].offset.y, 0.f)};
				Vector3f objRectPos{objRect->m_pos + Vector3f(objRect->m_colliders[iRect].offset.x, objRect->m_colliders[iRect].offset.y, 0.f)};
				Vector3f vecToRect{objRectPos - objRadialPos};
				float distanceToRect = length(vecToRect);

				// If distance from RadialCollider to RectCollider is smaller than collision radius, collision must be true
				if(distanceToRect < objRadial->m_colliders[iRad].radius)
				{
					return true;
				}
				else // Else find point on radius-perimeter nearest to rect and test if that point is within rect
				{
					Vector3f testPoint = objRadialPos + ((vecToRect / distanceToRect) * objRadial->m_colliders[iRad].radius);
					if (   testPoint.x < objRectPos.x + objRect->m_colliders[iRect].extents.x
						&& testPoint.x > objRectPos.x - objRect->m_colliders[iRect].extents.x
						&& testPoint.y < objRectPos.y + objRect->m_colliders[iRect].extents.y
						&& testPoint.y > objRectPos.y - objRect->m_colliders[iRect].extents.y)
					{
						return true;
					}
				}
			}

			// Rect v Rect
			else if (m_colliders[0].type == CollisionMode::COLL_RECT && obj->m_colliders[0].type == CollisionMode::COLL_RECT)
			{
				PLAY_ASSERT_MSG(false, "Collision scenario not implemented!");
			}
		}
	}

	// No collider matchups resulted in collision
	return false;
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

	for(int i = 0; i < m_colliders.size(); i++)
	{
		if(m_colliders[i].type == CollisionMode::COLL_RADIAL)
		{
			Graphics::SetMaterial(collMat);
			Graphics::DrawMesh(collMesh, MatrixTranslate<f32>(m_pos.x + m_colliders[i].offset.x, m_pos.y + m_colliders[i].offset.y, m_pos.z)
									   * MatrixScale<f32>(m_colliders[i].radius, m_colliders[i].radius, m_colliders[i].radius));
		}
		else if (m_colliders[i].type == CollisionMode::COLL_RECT)
		{
			Vector3f origin{m_pos + Vector3f(m_colliders[i].offset.x, m_colliders[i].offset.y, 0.f)};
			Graphics::DrawQuad(
				origin + Vector3f(-m_colliders[i].extents.x, m_colliders[i].extents.y, 0.f),
				origin + Vector3f(-m_colliders[i].extents.x, -m_colliders[i].extents.y, 0.f),
				origin + Vector3f(m_colliders[i].extents.x, m_colliders[i].extents.y, 0.f),
				origin + Vector3f(m_colliders[i].extents.x, -m_colliders[i].extents.y, 0.f),
				Colour::Blue
			);
		}
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