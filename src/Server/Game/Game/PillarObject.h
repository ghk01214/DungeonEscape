#pragma once

#include "GameObject.h"

class RigidBody;

class PillarObject : public GameObject
{
public:
	PillarObject(const Vec3& position = Vec3(0, 0, 0), const Quat& rotation = Quat(0, 0, 0, 1), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~PillarObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void Init_After_ColliderAttached();
	void ApplyRequestedLayers();
	void ReceivedAttack_SingleAttack();		//TriggerObj에 의한 공격
	void ReceivedAttack_Meteor();				//SkillObj에 의한 공격

public:		//server
	void ServerMessage_Init();
	void ServerMessage_Release();			//SetRemoveReserved()이후 바로 호출해야하는 함수

	bool GetRequireFlagTransmit();
	void SetRequireFlagTransmit(bool set);

	void Reset();

private:
	RigidBody* m_body = nullptr;

private:	//server
	bool m_requiresPacketTransmit = false;


	physx::PxTransform m_originalTrans;
};




//PilliarObject

