#pragma once

#include "UI_Script.h"

class Bar_Script : public UI_Script
{
public:
	Bar_Script();
	virtual ~Bar_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

protected:
	Vec3 m_pos;
	Vec3 m_scale;
	Vec2 m_scaleRatio;
};

