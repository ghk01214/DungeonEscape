#pragma once

#include "Scene.h"
#include "MonoBehaviour.h"

class Scene_Test
{
	DECLARE_SINGLE(Scene_Test)

public:
	std::shared_ptr<CScene> TestScene(void);

public:
	std::vector<std::shared_ptr<CGameObject>> CreateObject(ObjectDesc& objectDesc);
};