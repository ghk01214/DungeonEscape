#pragma once

#include "Scene.h"
#include "MonoBehaviour.h"

class Scene_Test
{
	DECLARE_SINGLE(Scene_Test)

public:
	std::shared_ptr<CScene> TestScene(void);

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

public:
	std::vector<std::shared_ptr<CGameObject>> CreateMapObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> CreateAnimatedObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> AddNetworkTodObject(std::vector<std::shared_ptr<CGameObject>> object, network::OBJECT_TYPE objectType);

private:
	shared_ptr<CScene> scene;
};