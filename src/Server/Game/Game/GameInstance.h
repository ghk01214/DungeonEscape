#pragma once

class TimeManager;
class ObjectManager;
class PhysDevice;
class EventHandler;

class TestLevel;

class GameInstance
{
public:
	DeclareSingletone(GameInstance)

private:
	GameInstance();
	~GameInstance();

public:
	void Init();
	void Update(double timeDelta, int interExtrapolation);
	void LateUpdate(double timeDelta, int interExtrapolation);
	void Release();

	ObjectManager*	m_objectManager = nullptr;
	TimeManager*	m_timeManager = nullptr;
	PhysDevice*		m_physDevice = nullptr;
	EventHandler*	m_eventHandler = nullptr;

	//추후 levelManager로 대체
	TestLevel*		m_testLevel = nullptr;
};

