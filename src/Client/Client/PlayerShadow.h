#pragma once

#include "MonoBehaviour.h"

class PlayerShadow : public MonoBehaviour
{
public:
	PlayerShadow();
	~PlayerShadow();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

};