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
	void CreateLights(void);

private:
	void CreateBackground();
	void CreateTitle();
	void CreateLogInButton();
	void CreateSettingButton();

	void CreatePopUp();
	void CreateBlur();
	void CreateCloseButton();
	void CreateSettingFrame();
	void CreateBGMButton();
	void CreateBGMSlider();
	void CreateSEButton();
	void CreateSESlider();

private:
	void ChangePopUpVisibility();
	void ChangeVolume();
	void ChangeMuteTexture();

public:
	static std::shared_ptr<CScene> Create();

private:
	std::shared_ptr<class LoginButton_Script> m_logInButton;
	std::shared_ptr<class Setting_Script> m_settingButton;
	std::shared_ptr<class CloseButton_Script> m_closeButton;
	std::vector<std::shared_ptr<class SliderTip_Script>> m_sliderTip;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSlider;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSliderLeftTip;
	std::vector<std::shared_ptr<class Mute_Script>> m_muteButton;

	std::vector<std::shared_ptr<CGameObject>> m_popUp;

	bool m_openSetting;
};