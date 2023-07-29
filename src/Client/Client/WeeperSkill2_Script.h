#pragma once

#include "MonsterRangeAttack.h"

class WeeperSkill2_Script : public MonsterRangeAttack
{
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void ParsePackets();
};

