#pragma once

#include "Button_Script.h"

class Close_Script : public Button_Script
{
public:
	Close_Script();
	virtual ~Close_Script();

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

