#pragma once

#include "GameObject.h"

class RigidBody;

class ArtifactObject : public GameObject
{
public:
	ArtifactObject(const Vec3& position = Vec3(0, 0, 0), const Quat& rotation = Quat(0, 0, 0, 1), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~ArtifactObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void ApplyRequestedLayers();
	void ReceivedAttack_SingleAttack();		//TriggerObj에 의한 공격
	void SetPillarWeak();					//돌기둥 약화 명령

public:		//server
	void ServerMessage_Init();
	void ServerMessage_Release();			//SetRemoveReserved()이후 바로 호출해야하는 함수

	void ServerFunctionHere();				//이름은 좋을대로 변경
private:
	RigidBody* m_body = nullptr;
	int m_requestedHitCnt = 5;				//이 횟수만큼 맞아야 사라지게 한다.
};




//PilliarObject

