#pragma once

#include "MonoBehaviour.h"

class Camera_Basic : public MonoBehaviour
{
public:
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	float		_speed = 500.f;
};

