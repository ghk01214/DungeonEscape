#pragma once

#include "Client_Defines.h"
#include "Scene.h"
#include "MonoBehaviour.h"

#include <unordered_set>

class Scene_Test final : public CScene
{
public:
	explicit Scene_Test();
	virtual ~Scene_Test() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

private:
	void CreateLayer(void);
	void CreateComputeShader(void);
	void CreateMainCamera(shared_ptr<CScene> pScene);
	void CreateUICamera(shared_ptr<CScene> pScene);
	void CreateSkyBox(shared_ptr<CScene> pScene);
	void CreateUI(shared_ptr<CScene> pScene);
	void CreateLights(shared_ptr<CScene> pScene);
	void CreateMap(shared_ptr<CScene> pScene);
	void CreatePlayer(shared_ptr<CScene> pScene, server::FBX_TYPE player);
	void CreateSphere(shared_ptr<CScene> pScene);

	void SendKeyInput();

	void ChangeNetworkObjectID(network::CPacket& packet);
	void CreateAnimatedRemoteObject(network::CPacket& packet);
	void CreateRemoteObject(network::CPacket& packet);
	void RemoveObject(network::CPacket& packet);

	void ClassifyObject(int32_t stateIndex, server::FBX_TYPE type, ObjectDesc& objectDesc);
	void AddObjectToScene(server::OBJECT_TYPE type, std::vector<std::shared_ptr<CGameObject>>& gameObjects);

public:
	std::vector<std::shared_ptr<CGameObject>> CreateMapObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> CreateAnimatedObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> AddNetworkToObject(std::vector<std::shared_ptr<CGameObject>> object, server::OBJECT_TYPE objectType, int32_t id = -1);

private:
	void Init(shared_ptr<Scene_Test> pScene, server::FBX_TYPE eType);

private:
	std::unordered_map<int32_t, int32_t> m_objectIDMap;		// temp obj id, new obj id

	std::unordered_set<int32_t> m_overlappedObjects;

public:
	static shared_ptr<CScene> Create(server::FBX_TYPE eType);
};