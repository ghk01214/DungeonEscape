﻿#pragma once

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
	void Tick(double timeDelta);

public:
	void Init();
	void Update(double timeDelta);
	void LateUpdate(double timeDelta);
	void Release();

public:
	void AddEvent(std::string context, float remainTime, GameObject* subject);
	void AddEventIfNone(std::string context, float remainTime, GameObject* subject);
	Event* GetEvent(std::string);
	void DeleteEvent(std::string context);

private:
	std::vector<Event*> m_events;
};

