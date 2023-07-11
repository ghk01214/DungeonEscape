#pragma once

#include "Client_Defines.h"
#include "Scene.h"

class Scene_Loading : public CScene
{
public:
	explicit Scene_Loading();
	virtual ~Scene_Loading() = default;

public:
	void Loading(SCENE eNextScene);

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

private:
	SCENE			m_eNextScene = SCENE_END;
	shared_ptr<class CLoader> m_pLoader = nullptr;

public:
	static shared_ptr<CScene> Create(SCENE eNextScene);
};