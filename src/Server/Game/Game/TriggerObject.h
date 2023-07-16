#pragma once

#include "GameObject.h"

class RigidBody;
class Collider;

class TriggerObject : public GameObject
{
	friend class Player;
	friend class Monster;
	friend class Golem;
	friend class Event;
public:
	enum class SKILLTYPE
	{
		NONE,

		PLAYER_NANA_ATTACK1,
		PLAYER_NANA_ATTACK2,
		PLAYER_NANA_ATTACK3,
		PLAYER_NANA_ATTACK4,
		PLAYER_NANA_ATTACK5,

		BOUNDARY, 

		GOLEM_ATTACK1,
		GOLEM_ATTACK2,
		GOLEM_ATTACK3,
		GOLEM_ATTACK4,
	};

public:
	TriggerObject(const Vec3& position = Vec3(0, 0, 0), const Quat& rotation = Quat(0, 0, 0, 1), const Vec3& scale = Vec3(1.f, 1.f, 1.f), SKILLTYPE type = SKILLTYPE::NONE);
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
	void Trigger_SingleStrike_HandlePlayers();

public:
	void ExcludeTriggerFromSimulation(bool value);
	void ExcludeTriggerFromSimulation_Ver2(bool value);
	void ExcludeManagement(Collider* collider, bool& duplicate);
	void RestoreOneTimeEffect();

public:
	void SetTriggerType(server::TRIGGER_TYPE, float startTime = -1.f, float endTime = -1.f);
	server::TRIGGER_TYPE GetTriggerType();
private:
	RigidBody* m_body = nullptr;

private:
	server::TRIGGER_TYPE m_triggerType = server::TRIGGER_TYPE::NONE;
	SKILLTYPE m_skillType = SKILLTYPE::NONE;
	std::vector<Collider*> m_excludeColliders;		//중복처리 방지용 컨테이너(1회성 트리거 한정)
	float m_startTime = -1.f;
	float m_endTime = -1.f;			
	float m_currentTime = -1.f;						//트리거 활성화 시간 + 시작/끝(1회성 트리거 한정)

	physx::PxVec3 m_originalPosition = physx::PxVec3(0);

	bool m_simulationExcluded = false;
};



