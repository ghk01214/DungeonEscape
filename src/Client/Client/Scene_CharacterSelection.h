#pragma once

#include "Client_Defines.h"
#include <Scene.h>

class Scene_CharacterSelection final : public CScene
{
private:
	enum CHARACTER_TYPE
	{
		NANA = 2,
		MISTIC,
		CARMEL,

		MAX
	};
public:
	explicit Scene_CharacterSelection();
	virtual ~Scene_CharacterSelection() = default;

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;
	void FinalUpdate() override;
	void Render() override;

private:
	void Init();
	void CreateLayer();
	void CreateUICamera();
	void CreateUI();
	void CreateLights();

private:
	void CreateCharacterImage();
	void CreateCharacterNameButton();
	void CreateReadyButton();

	void CreatePopUp();
	void CreateBlur();
	void CreateCloseButton();
	void CreateCharacterDescription();

private:
	server::FBX_TYPE DetectCharacterSelection();
	void DeactivateCharacterUI();
	void StartGame(server::FBX_TYPE character);
	void ChangePopUpVisibility();
	void SetUIActive(bool flag);

public:
	static std::shared_ptr<CScene> Create();

private:
	std::vector<std::shared_ptr<class CharacterSelectButton_Script>> m_characterButton;
	std::vector<std::shared_ptr<class DescriptionButton_Script>> m_descriptionButton;
	std::shared_ptr<class ReadyButton_Script> m_readyButton;

	std::vector<std::shared_ptr<CGameObject>> m_popUp;
	std::vector<std::shared_ptr<CGameObject>> m_characterDescription;
	std::shared_ptr<class CloseButton_Script> m_closeButton;

private:
	bool m_selected;
	float m_accTime;
	bool m_popUpActive;
	int32_t m_characterIndex;
	int32_t m_descriptionIndex;
};