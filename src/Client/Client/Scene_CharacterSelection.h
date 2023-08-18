#pragma once

#include "Client_Defines.h"
#include <Scene.h>

class CharacterSelection_Button;

class Scene_CharacterSelection final : public CScene
{
public:
	explicit Scene_CharacterSelection();
	virtual ~Scene_CharacterSelection() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

private:
	void Init();
	void CreateLayer();
	void CreateUICamera();
	void CreateUI();
	void LoadTextures();
	void CreateLights();

private:
	void CreateCharacterImage();
	void CreateCharacterNameButton();
	void CreateReadyButton();

public:
	static std::shared_ptr<CScene> Create();

private:
	std::vector<std::shared_ptr<CharacterSelection_Button>> m_buttons;

	bool m_selected;
};