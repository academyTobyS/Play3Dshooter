#pragma once
#include "GameObject.h"

class GameObject;

class GameObjectManager
{
public:
	GameObjectManager() = default;
	~GameObjectManager();

	GameObject* CreateObject( GameObjectType objType, Play3d::Vector3f pos);
	void RegisterGameObject( GameObject* obj ) { m_pGameObjectList.push_back( obj ); };
	
	// Load item into memory if not already loaded, return Play3d ID
	Play3d::Graphics::MeshId GetMesh(const char* filepath);
	Play3d::Graphics::MaterialId GetMaterial(const char* textureFilepath = "");
	Play3d::Graphics::MaterialId GetMaterialHLSL(const char* hlslPath, const char* texturePath = "");

	void UpdateAll();
	void DrawAll();
	void DrawCollisionAll();
	void CollideAll();
	void CleanUpAll(); 

	GameObject* GetPlayer() { return m_pPlayer; }
	void SetPlayer( GameObject* pPlayer ) { m_pPlayer = pPlayer; }
	int GetAllObjectsOfType( GameObjectType objType, std::vector<GameObject*>& objList, bool clearList = true );
	void DeleteGameObjectsByType( GameObjectType type );

private:
	std::vector<GameObject*> m_pGameObjectList;
	std::unordered_map<const char*, Play3d::Graphics::MeshId> m_meshRegister;
	std::unordered_map<const char*, Play3d::Graphics::MaterialId> m_materialRegister;
	GameObject* m_pPlayer{ nullptr };
};

GameObjectManager* GetObjectManager();
void DestroyObjectManager();
