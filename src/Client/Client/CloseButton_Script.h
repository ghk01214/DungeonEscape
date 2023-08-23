#pragma once

#include "Button_Script.h"

class CloseButton_Script : public Button_Script
{
public:
	CloseButton_Script();
	virtual ~CloseButton_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	constexpr bool ClosePopUp() const { return m_closePopUp; }

	void SetClosePopUpFlag(bool flag);

private:
	bool m_closePopUp;
};

