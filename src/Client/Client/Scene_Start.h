#pragma once

#include "Client_Defines.h"
#include <Scene.h>

class Scene_Start final : public CScene
{
private:
	enum SLIDER_TYPE
	{
		BGM,
		SE,

		MAX
	};
public:
	explicit Scene_Start();
	virtual ~Scene_Start() = default;

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
	void CreateBackground();
	void CreateTitle();
	void CreateLogInButton();
	void CreateSettingButton();

	void CreatePopUp();
	void CreateBlur();
	void CreateCloseButton();
	void CreatePopUpFrame();
	void CreateBGMButton();
	void CreateBGMSlider();
	void CreateSEButton();
	void CreateSESlider();
	void CreateLogInInputField();
	void CreatePopUpLogInButton();

private:
	void ChangeScene();
	void ChangePopUpVisibility();
	void ChangeVolume();
	void ChangeMuteTexture();

private:
	void CreateSample();
	void CreateSample2(float yPos);
	void CreateBossSample();

	std::vector<std::shared_ptr<CGameObject>> m_ui;
	float m_centerPos;

public:
	static std::shared_ptr<CScene> Create();

private:
	std::shared_ptr<class LoginButton_Script> m_logInPopUpButton;
	std::shared_ptr<class LoginButton_Script> m_logInButton;
	std::shared_ptr<class SettingButton_Script> m_settingButton;
	std::shared_ptr<class CloseButton_Script> m_closeButton;
	std::vector<std::shared_ptr<class SliderTip_Script>> m_sliderTip;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSlider;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSliderLeftTip;
	std::vector<std::shared_ptr<class MuteButton_Script>> m_muteButton;
	std::shared_ptr<class InputString_Script> m_inputField;

	std::vector<std::shared_ptr<CGameObject>> m_settingPopUp;
	std::vector<std::shared_ptr<CGameObject>> m_logInPopUp;

	bool m_openSetting;
	bool m_openLogIn;
};