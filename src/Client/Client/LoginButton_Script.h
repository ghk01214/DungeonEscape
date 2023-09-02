#pragma once

#include "Button_Script.h"

class LoginButton_Script final : public Button_Script
{
public:
	LoginButton_Script(bool active = true);
	virtual ~LoginButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	constexpr bool IsLogIn() const { return m_logIn; }
	constexpr bool ShowPopUp() const { return m_showPopUp; }

	void SetShowPopUpFlag(bool flag);
	void SetTemp(int32_t t) { temp = t; }

private:
	bool m_showPopUp;
	bool m_logIn;
	int32_t temp;
};