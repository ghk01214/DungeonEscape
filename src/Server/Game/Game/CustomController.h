#pragma once
#include "physx_define.h"

class RigidBody;
class CapsuleCollider;

class CustomController
{
public:
	CustomController();
	~CustomController();
public:
	void Init(physx::PxVec3 pos);
	void SetRigidBody(RigidBody* body);

public:
	void Update(uint8_t keyInput, server::KEY_STATE keyState);
	bool CheckOnGround(CollisionInfoType type, physx::PxVec3& surfaceNormal);
	void GetSlidingVector(CollisionInfoType type);
	bool CheckOnGround_Raycast();
	void DirectionInput(uint8_t keyInput);
	void Move(uint8_t keyInput, server::KEY_STATE keyState);
private:
	physx::PxVec3	m_moveDirection{ 0.f };
	physx::PxVec3	m_slidingVector{ 0.f };

	bool	m_onGround = false;
	int		m_debugPrint = 0;
	float	m_degreeThreshold = 49.9f;
private:
	RigidBody*			m_body = nullptr;
	CapsuleCollider*	m_collider = nullptr;
};


