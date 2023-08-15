#pragma once

#include "Client_Defines.h"
#include <Scene.h>

class Scene_Start final : public CScene
{
public:
	explicit Scene_Start();
	virtual ~Scene_Start() = default;

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

private:
	void CreateBackground(float width, float height);
	void CreateTitle(float width, float height);
	void CreateLogInButton(float width, float height);

public:
	static std::shared_ptr<CScene> Create();

private:
	std::wstring m_ipAddress;
};