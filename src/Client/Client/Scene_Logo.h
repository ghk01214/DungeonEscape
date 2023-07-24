#pragma once

#include "Client_Defines.h"
#include "Scene.h"

class Scene_Logo final : public CScene
{
public:
	explicit Scene_Logo();
	virtual ~Scene_Logo() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

private:
	void Init(void);
	void CreateLayer(void);
	void CreateUICamera(void);
	void CreateUI(void);
	void LoadTextures(void);
	void CreateLights(void);

public:
	static shared_ptr<CScene> Create();
};