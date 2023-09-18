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
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;
	void FinalUpdate() override;
	void Render() override;

public:
	void Init();
	void CreateLayer();
	void CreateUICamera();
	void CreateUI();
	void LoadTextures();
	void CreateLights();

private:
	void ReadServerIPAddress(std::wstring& ipAddress);

public:
	static std::shared_ptr<CScene> Create(SCENE eNextScene);

private:
	SCENE m_eNextScene;
	std::shared_ptr<class CLoader> m_pLoader;

	server::FBX_TYPE m_character;
	//std::vector<std::shared_ptr<Texture>> m_textures;
};