#pragma once

#include "Logo_Script.h"

class Logo_Start final : public Logo_Script
{
public:
	Logo_Start();
	virtual ~Logo_Start();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;
};