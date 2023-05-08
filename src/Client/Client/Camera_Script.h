#pragma once

#include "MonoBehaviour.h"

class Camera_Basic : public MonoBehaviour
{
public:
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	float		_speed = 2000.f;
	float		m_lengthX = 1000.f;
	float		m_lengthY = 600.f;
	float		m_rotation = 180.f;
};

