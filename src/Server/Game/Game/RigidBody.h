#pragma once
#include "physx_define.h"
#include "Component.h"

class Collider;
class Transform;

class RigidBody : public Component
{
	friend class Collider;

public:
	RigidBody(GameObject* ownerGameObject, Component* ownerComponent);
	~RigidBody();

public:
	void Init() override;
	void Release() override;

public:
	//// 의도한 삭제가 진행되었는지 알기 위해서 타입을 명시하도록 합니다.
	template<typename ColliderType, typename... Args>
	ColliderType* AddCollider(Args&& ...args);

	template<typename ColliderType>
	ColliderType* GetCollider(int index);

	template<typename ColliderType>
	void RemoveCollider(ColliderType* colliderToRemove);

private:
	//AddComp,RemoveComp에서 작동되는 코드입니다. 외부에서 사용을 금합니다.
	void Attach(Collider* collider);
	void Detach(Collider* collider);
	void ApplyFlags();

public:
	physx::PxRigidDynamic* GetBody();
	Collider* GetCollider(int index);

public:
	// 프레임워크 구현되는대로 맞게 인자 수정
	physx::PxVec3 GetPosition();
	void SetPosition(const Vec3 position, bool sleep);
	void SetRotation(const Quat rotation);
	void SetRotation(const float degree, PhysicsAxis axis);

	void SetTransfrom(const Transform* transform);

public:
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

