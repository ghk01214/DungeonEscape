#pragma once

#include "Button_Script.h"

class Start_StartButton : public Button_Script
{
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