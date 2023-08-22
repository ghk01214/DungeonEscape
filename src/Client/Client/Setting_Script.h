#pragma once

#include "Button_Script.h"

class Setting_Script : public Button_Script
{
public:
	Setting_Script();
	virtual ~Setting_Script();

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

