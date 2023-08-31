#pragma once

class GameObject;
class EventHandler;
class SkillObject;

class Event
{
	friend class SkillObject;
	friend class EventHandler;
public:
	Event(std::string context, double remainTime, GameObject* subject);
	~Event();

	void Tick(double timeDelta);			//시간만 소모한다
	void Tick_TimeInterval();			//실제로 명령을 수행한다.
	void ExecuteMsg_Once();
	void ExecuteMsg_continuous();

public:
	std::string			msg;
	double 				time;
	GameObject*			target;
	bool				executed = false;
};
