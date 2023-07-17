#pragma once

class GameObject;
class EventHandler;
class SkillObject;

class Event
{
	friend class SkillObject;
	friend class EventHandler;
public:
	Event(std::string context, float remainTime, GameObject* subject);
	~Event();

	void Tick(float timeDelta);
	void ExecuteMsg_Once();		//Execute 함수는 가독성을 위해 분리
	void ExecuteMsg_continuous();		

public:
	std::string			msg;
	float				time;
	GameObject*			target;
	bool				executed = false;
};

//SKILL_GUIDESTART

