#pragma once

#include "Button_Script.h"

class SettingButton_Script : public Button_Script
{
public:
	SettingButton_Script();
	virtual ~SettingButton_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	constexpr bool ShowPopUp() const { return m_showPopUp; }

	void SetShowPopUpFlag(bool flag);

private:
	bool m_showPopUp;
};

