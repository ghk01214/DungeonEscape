#pragma once

#include "MonoBehaviour.h"

class Camera_Basic : public MonoBehaviour
{
public:
	Camera_Basic();
	~Camera_Basic();

public:
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	float m_speed;
	float m_lengthX;
	float m_lengthY;
	float m_rotation;
};

