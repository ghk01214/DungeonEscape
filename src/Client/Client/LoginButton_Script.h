#pragma once

#include "Button_Script.h"

class LoginButton_Script : public Button_Script
{
public:
	LoginButton_Script();
	virtual ~LoginButton_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	bool m_lobbyEnd;
};