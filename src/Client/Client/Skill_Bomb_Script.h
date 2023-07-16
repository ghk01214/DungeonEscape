#pragma once

#include "Skill_Script.h"

class Bomb_Script : public Skill_Script
{
public:
	Bomb_Script();
	virtual ~Bomb_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	float m_AlphaChangeSpeed = 0.5f;
	float m_AlphaChangeDegree = -1.f;
};