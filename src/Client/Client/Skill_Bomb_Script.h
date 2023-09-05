#pragma once

#include "Skill_Script.h"

class Bomb_Script : public Skill_Script
{
public:
	Bomb_Script(float scaleSpeed = 1.f, float alpha = 1.f);
	virtual ~Bomb_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	float m_alphaChangeSpeed;
	float m_alphaChangeDegree;

	float m_scaleChangeSpeed;
};