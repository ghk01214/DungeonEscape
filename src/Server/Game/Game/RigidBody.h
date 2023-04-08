#pragma once
#include "physx_utils.h"

enum class Interpolate;
enum class PhysicsAxis;
enum class ForceMode;
enum class ColliderShape;

class RigidBody
{
	friend class Collider;

public:
	void Init(ColliderShape shape);
	void ApplyFlags();
public:
	void AttachAll();
	void DetachAll();

	void Attach(Collider* collider);
	void Detach(Collider* collider);

public:
	physx::PxRigidDynamic* GetBody();

	Collider* GetCollider(int index);

public:
	// 프레임워크 구현되는대로 맞게 인자 수정
	physx::PxVec3 GetPosition();
	void SetPosition(const float x, const float y, const float z, bool sleep);
	void SetRotation(float degree, PhysicsAxis axis);


	physx::PxVec3 GetVelocity() const;
	void SetVelocity(const physx::PxVec3& velocity);
	float GetMass() const;
	void SetMass(float value);
public:
	void UpdateMassAndInertia();
	bool IsRigidbodySleep() const;
	void SetRigidbodySleep(bool value);
	float GetSleepThresholder() const;
	void SetSleepThresholder(float value);

public:
	void SetRotationLockAxis(PhysicsAxis axes, bool value);
	void SetKinematic(bool value);
	void SetCCDFlag(bool value);
	bool isKinematic() const;

private:
	physx::PxRigidDynamic* m_body;
	bool m_continuous{ true };

	std::vector<Collider*> m_colliders;
};

