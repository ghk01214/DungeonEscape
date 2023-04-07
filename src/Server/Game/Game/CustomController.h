#pragma once
#include "physx_utils.h"

class RigidBody;
class CapsuleCollider;

enum class CollisionInfoType;

class CustomController
{
public:
	CustomController();
	~CustomController();
public:
	void Init();
	void SetRigidBody(RigidBody* body);

public:
	void Update();
	bool CheckOnGround(CollisionInfoType type, physx::PxVec3& surfaceNormal);
	void GetSlidingVector(CollisionInfoType type);
	bool CheckOnGround_Raycast();
	void DirectionInput();
	void GetAdjustedVector(CollisionInfoType type);
	void Move();
private:
	physx::PxVec3	m_moveDirection{ 0.f };
	physx::PxVec3	m_slidingVector{ 0.f };

	bool	m_onGround = false;
	int		m_debugPrint = 0;
	float	m_degreeThreshold = 49.9f;
private:
	RigidBody* m_body = nullptr;
	CapsuleCollider* m_collider = nullptr;
};


