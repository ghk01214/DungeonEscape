#pragma once

#include "Client_Defines.h"
#include "Scene.h"

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
	void Init(void);
	void CreateLayer(void);
	void CreateUICamera(void);
	void CreateUI(void);
	void LoadTextures(void);
	void CreateLights(void);

public:
	static std::shared_ptr<CScene> Create();

private:
	std::vector<std::shared_ptr<CharacterSelection_Button>> m_buttons;

	bool m_selected;
};