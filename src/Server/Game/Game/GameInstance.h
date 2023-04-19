#pragma once

class TimeManager;
class ObjectManager;
class PhysDevice;

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

	ObjectManager*	m_objectManager = nullptr;
	TimeManager*	m_timeManager = nullptr;
	PhysDevice*		m_physDevice = nullptr;
};

