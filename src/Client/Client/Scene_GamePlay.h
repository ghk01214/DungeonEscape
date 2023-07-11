#pragma once

#include "Client_Defines.h"
#include "Scene.h"

class Scene_GamePlay final : public CScene
{
public:
	explicit Scene_GamePlay();
	virtual ~Scene_GamePlay() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

public:
	static shared_ptr<CScene> Create();
};