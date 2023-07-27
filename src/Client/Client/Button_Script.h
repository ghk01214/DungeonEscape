#pragma once

#include "UI_Script.h"

class Button_Script : public UI_Script
{
public:
	Button_Script();
	virtual ~Button_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

protected:
	class CInput* m_input;

	Vec3 m_pos;
	Vec3 m_scale;
	Vec2 m_scaleRatio;
	POINT m_mousePos;

	bool m_click;
};