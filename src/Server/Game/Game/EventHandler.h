#pragma once

class Event;
class GameObject;

class EventHandler
{
	friend class Event;
public:
	DeclareSingletone(EventHandler)

private:
	EventHandler();
	virtual ~EventHandler() = default;

public:
	void Init();
	void Update(double timeDelta);
	void LateUpdate(double timeDelta);
	void Release();

public:
	void AddEvent(std::string context, float remainTime, GameObject* subject);

private:
	std::vector<Event*> m_events;
};

