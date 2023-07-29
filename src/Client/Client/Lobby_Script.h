#pragma once

#include "UI_Script.h"

class Lobby_Script : public UI_Script
{
public:
	Lobby_Script();
	virtual ~Lobby_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

};