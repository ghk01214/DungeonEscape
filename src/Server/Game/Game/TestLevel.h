#pragma once

#include "Level.h"

class TestLevel : public Level
{
public:
	TestLevel();
	virtual ~TestLevel() override;

public:
	virtual void Init() override;
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release() override;
};

