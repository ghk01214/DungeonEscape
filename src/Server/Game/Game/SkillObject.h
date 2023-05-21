#pragma once

#include "GameObject.h"

class RigidBody;

class SkillObject : public GameObject
{
public:
	//projectile 위주. 무조건 위치 전달
	SkillObject(const Vec3& position = Vec3(0, 0, 0), const Quat& rotation = Quat(0, 0, 0, 1), const Vec3& scale = Vec3(1.f, 1.f, 1.f), server::OBJECT_TYPE type = server::OBJECT_TYPE::FIREBALL);
	~SkillObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void ActionsWhenHit();		//충돌했는가 판단
private:
	RigidBody* m_body = nullptr;
	server::OBJECT_TYPE m_objType = server::OBJECT_TYPE::FIREBALL;
};



