#pragma once

#include "MonoBehaviour.h"

class Effect_Script : public MonoBehaviour
{
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
};

