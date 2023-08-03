#pragma once

#include "Client_Defines.h"
#include <Scene.h>

class Scene_Loading final : public CScene
{
public:
	explicit Scene_Loading();
	virtual ~Scene_Loading() = default;

public:
	void Loading(SCENE eNextScene);
	void LogIn();

	std::shared_ptr<class CLoader> GetLoader() { return m_pLoader; }

	void SetSelectedCharacter(server::FBX_TYPE character);

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

public:
	void Init();
	void CreateLayer();
	void CreateUICamera();
	void CreateUI();
	void LoadTextures();
	void CreateLights();

private:
	SCENE			m_eNextScene = SCENE_END;
	std::shared_ptr<class CLoader> m_pLoader = nullptr;

	server::FBX_TYPE m_character;
	//std::vector<std::shared_ptr<Texture>> m_textures;

public:
	static shared_ptr<CScene> Create(SCENE eNextScene);
};