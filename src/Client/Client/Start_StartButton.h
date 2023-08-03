﻿#pragma once

#include "Button_Script.h"

class Start_StartButton : public Button_Script
{
public:
	enum TEXTURE_TYPE
	{
		START_BUTTON,
		START_BUTTON_PRESSED,

		MAX
	};
public:
	Start_StartButton();
	virtual ~Start_StartButton();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	bool m_lobbyEnd;
};