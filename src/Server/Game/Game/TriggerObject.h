#pragma once

#include "GameObject.h"

class RigidBody;
class Collider;

class TriggerObject : public GameObject
{
public:
	TriggerObject(const Vec3& position = Vec3(0, 0, 0), const Quat& rotation = Quat(0, 0, 0, 1), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~TriggerObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void TriggerUpdate();
	void Trigger_Persistent();
	void Trigger_SingleStrike_HandleMonster();
	void Trigger_SingleStrike_HandlePillar();

public:
	void TimeUpdate(double deltaTime);

public:
	void ExcludeTriggerFromSimulation(bool value);
	void ExcludeManagement(Collider* collider, bool& duplicate);
	void RestoreOneTimeEffect();

public:
	void SetTriggerType(server::TRIGGER_TYPE, double startTime = -1.f, double endTime = -1.f);
	server::TRIGGER_TYPE GetTriggerType();
private:
	RigidBody* m_body = nullptr;

private:
	server::TRIGGER_TYPE m_triggerType = server::TRIGGER_TYPE::NONE;
	std::vector<Collider*> m_excludeColliders;		//중복처리 방지용 컨테이너(1회성 트리거 한정)
	double m_startTime = -1.f;
	double m_endTime = -1.f;
	double m_currentTime = -1.f;						//트리거 활성화 시간 + 시작/끝(1회성 트리거 한정)

	physx::PxVec3 m_originalPosition = physx::PxVec3(0);
};



