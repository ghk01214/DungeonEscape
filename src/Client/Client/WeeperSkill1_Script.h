#pragma once

#include "Effect_Script.h"

class WeeperSkill1_Script : public Effect_Script
{
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
};

