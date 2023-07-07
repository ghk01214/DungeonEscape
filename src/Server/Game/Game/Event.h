﻿#pragma once

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
	void ExecuteMsg_Once();

public:
	std::string			msg;
	float				time;
	GameObject*			target;
	bool				executed = false;
};

//SKILL_GUIDESTART

