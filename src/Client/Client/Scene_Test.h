﻿#pragma once

#include "Scene.h"
#include "MonoBehaviour.h"

class Scene_Test
{
	DECLARE_SINGLE(Scene_Test)

public:
	std::shared_ptr<CScene> TestScene(void);

	void LateUpdate();

private:
	void CreateLayer(void);
	void CreateComputeShader(void);
	void CreateMainCamera(void);
	void CreateUICamera(void);
	void CreateSkyBox(void);
	void CreateUI(void);
	void CreateLights(void);
	void CreateMap(void);
	void CreateMapObjects(void);
	void CreatePlayer(void);
	void CreateRemotePlayer(network::CPacket& packet);

	void ChangeNetworkObjectID(network::CPacket& packet);
	void CreateRemoteObject(network::CPacket& packet);
	void ChangeColliderID(network::CPacket& packet);
	void AddColliderToObject(network::CPacket& packet);

public:
	std::vector<std::shared_ptr<CGameObject>> CreateMapObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> CreateAnimatedObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> AddNetworkToObject(std::vector<std::shared_ptr<CGameObject>> object, server::OBJECT_TYPE objectType, int32_t id = -1);

private:
	shared_ptr<CScene> scene;
	std::unordered_map<int32_t, int32_t> m_objectIDMap;		// temp obj id, new obj id
};