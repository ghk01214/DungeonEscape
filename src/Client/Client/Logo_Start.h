#pragma once

#include "Logo_Script.h"

class Logo_Start : public Logo_Script
{
public:
	Logo_Start();
	virtual ~Logo_Start();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
};