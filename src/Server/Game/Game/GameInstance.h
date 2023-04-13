#pragma once

class GameInstance
{
public:
	DeclareSingletone(GameInstance)

private:
	GameInstance();
	virtual ~GameInstance() = default;

public:
	void Init();
	void Update(double timeDelta);
	void LateUpdate(double timeDelta);
	void Release();

	class TimeDevice* m_TimeDevice = nullptr;
};

