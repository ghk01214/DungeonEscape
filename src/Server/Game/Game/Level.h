#pragma once
class Level
{
public:
	Level() = default;
	virtual ~Level() = default;

public:
	virtual void Init() = 0;
	virtual void Update(double timeDelta) = 0;
	virtual void LateUpdate(double timeDelta) = 0;
	virtual void Release() = 0;
};

