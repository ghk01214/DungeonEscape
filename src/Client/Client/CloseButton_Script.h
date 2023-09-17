#pragma once

#include "Button_Script.h"

class CloseButton_Script final : public Button_Script
{
public:
	CloseButton_Script();
	virtual ~CloseButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	constexpr bool ClosePopUp() const { return m_closePopUp; }

	void SetClosePopUpFlag(bool flag);
	void SetVisible(bool flag);

private:
	bool m_closePopUp;
};

