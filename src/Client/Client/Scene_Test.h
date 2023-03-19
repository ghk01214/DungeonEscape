#pragma once

class CScene;
#include "MonoBehaviour.h"

class Scene_Test
{
	DECLARE_SINGLE(Scene_Test)

public:
	std::shared_ptr<CScene> TestScene(void);
};