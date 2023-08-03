#pragma once

#include "UI_Script.h"

class Start_Script : public UI_Script
{
public:
	Start_Script();
	virtual ~Start_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

};