#include "GameObject.h"
#include "ObjectManager.h"

// Needs to know about all the object types so it can create them
#include "ObjectPlayer.h"
#include "ObjectPellet.h"
#include "ObjectBoss.h"

// **************************************************************************************************
// These functions provide global access to the GameObjectManager class throughout the codebase
// This is a prototypical "singleton" design pattern, but avoiding the use of the 'static' keyword
// which causes a lot of confusion for new programmers
// **************************************************************************************************

// A global pointer to a GameObjectManager instance (not delared/visible outside of this compilation unit)
GameObjectManager* g_pObjMan = nullptr;

// Global access to the manager via a function which creates a new GetObjectManager on first use
GameObjectManager* GetObjectManager()
{
	if( !g_pObjMan )
		g_pObjMan = new GameObjectManager();

	return g_pObjMan;
}

// Global access to freeing up all the manager and all of its objects
void DestroyObjectManager()
{
	PLAY_ASSERT_MSG( g_pObjMan, "Trying to destroy non-existant GameObjectManager")
	delete g_pObjMan;
	g_pObjMan = nullptr;
}

// **************************************************************************************************
// The class implmentations for GameObjectManager continue from here
// **************************************************************************************************

GameObjectManager::~GameObjectManager()
{
	for( int i = 0; i < m_pGameObjectList.size(); i++ )
		delete m_pGameObjectList[ i ];

	m_pGameObjectList.clear();
}

// This is a factory pattern which decouples the creation of specific object types from their class implementations
// This means you can create any type of GameObject from external code via the GameObjectManager without needing 
// to reference the class itself. In practice this means a lot fewer #includes are necessary
GameObject* GameObjectManager::CreateObject(GameObjectType objType, Play3d::Vector3f pos)
{
	GameObject* pNewObj = nullptr;

	switch( objType )
	{
	case TYPE_PLAYER:
		pNewObj = new ObjectPlayer(pos);
		break;

	case TYPE_PLAYER_PELLET:
		pNewObj = new ObjectPellet(pos);
		break;

	case TYPE_BOSS:
		pNewObj = new ObjectBoss(pos);
		break;
	}

	if( pNewObj != nullptr )
		GetObjectManager()->RegisterGameObject( pNewObj );

	PLAY_ASSERT(pNewObj);
	return pNewObj;
}

// Use the list of registered GameObjects to update them all...
void GameObjectManager::UpdateAll()
{
	for( int i = 0; i < m_pGameObjectList.size(); i++ ) 
	{
		m_pGameObjectList[ i ]->StandardMovementUpdate();
		m_pGameObjectList[ i ]->Update();
	}
	CollideAll();
	CleanUpAll();
}

// Use the list of registered GameObjects to draw them all...
void GameObjectManager::DrawAll()
{
	for( int i = 0; i < m_pGameObjectList.size(); i++ ) 
	{
		if( !m_pGameObjectList[ i ]->IsHidden() )
		{
			GameObject& obj = *m_pGameObjectList[ i ]; 
			obj.Draw();
		}
	}
}

// Use the list of registered GameObjects to collide them all...
void GameObjectManager::CollideAll()
{
	// Compare every game object with every other game object ignoring duplicate comparisons
	// Not very efficent! O(n*(n-1)/2) => O(n2) so consider using hash maps and/or sweep and prune to reduce collision tests
	for( int i = 0; i < m_pGameObjectList.size(); i++ ) 
	{
		//      i  0  1  2  3  4 
		//   j -|---------------
		//   0  |  X  C  C  C  C  
		//   1  |  X  X  C  C  C  
		//   2  |  X  X  X  C  C 
		//   3  |  X  X  X  X  C 
		//   4  |  X  X  X  X  X 
		// 
		// There is no need to compare against objects 0-i becaue we've already done those (see above: C = collision test)
		for( int j = i+1; j < m_pGameObjectList.size(); j++ ) 
		{
			// Don't compare objects against objects of the same type
			if( m_pGameObjectList[ i ]->GetObjectType() != m_pGameObjectList[ j ]->GetObjectType() )
			{
				if( m_pGameObjectList[ i ]->IsColliding( m_pGameObjectList[ j ] ) )
				{
					// Need to call the OnCollision functions of BOTH objects as they are only compared once
					m_pGameObjectList[ i ]->OnCollision( m_pGameObjectList[ j ] );
					m_pGameObjectList[ j ]->OnCollision( m_pGameObjectList[ i ] );
				}
			}
		}
	}
}

// Remove any flagged objects from the list of registered GameObjects
// Doing this outside of the main update loop helps to avoid various problems that can occur by deleting mid-update
void GameObjectManager::CleanUpAll()
{
	for( int i = 0; i < m_pGameObjectList.size(); i++ )
	{
		if( m_pGameObjectList[ i ]->IsDestroyed() ) 
		{
			delete m_pGameObjectList[ i ];
			m_pGameObjectList.erase( find( m_pGameObjectList.begin(), m_pGameObjectList.end(), m_pGameObjectList[ i-- ] ) );
		}
	}
}

int GameObjectManager::GetAllObjectsOfType( GameObjectType objType, std::vector<GameObject*>& objList, bool clearList )
{
	if( clearList == true )
		objList.clear();

	int count = 0;
	for( int i = 0; i < m_pGameObjectList.size(); i++ )
	{
		if( m_pGameObjectList[ i ]->GetObjectType() == objType )
		{
			objList.push_back( m_pGameObjectList[ i ] );
			count++;
		}
	}

	return count;
}

void GameObjectManager::DeleteGameObjectsByType( GameObjectType type )
{
	for( int i = 0; i < m_pGameObjectList.size(); i++ )
	{
		if( m_pGameObjectList[ i ]->GetObjectType() == type )
			m_pGameObjectList[ i ]->Destroy();
	}
}