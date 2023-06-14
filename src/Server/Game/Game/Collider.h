#pragma once

#pragma region old note
//	shape의 extent 변경은 해당 shape를 사용하는 모든 게임 오브젝트에 영향을 준다.
//	그 외의 요소(mat의 반발,마찰 계수 + shape의 filterData + mat/shape의 flag는 과거에 생성한 결과물에는 영향을 안 준다)
//	restitution(반발력)은 mat, shape 동시에 있다. 보통은 shape의 반발력 계수에 차이를 두는것으로 충분하나
//	mat을 공유하는 오브젝트의 반발계수를 한꺼번에 조정하기 위해 사용해도 좋다.
#pragma endregion

#include "physx_define.h"
#include "Component.h"

class RigidBody;
class CollisionPairInfo;
class TriggerPairInfo;

class Collider : public Component
{
	friend class RigidBody;

public:
	Collider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body);
	~Collider();

public:
	virtual void Init() override;
	virtual void Release() override;

	void ApplyShapeFlags();
	void ApplyTransform();
	void ApplyScale();
	void ApplyLayer();

protected:
	virtual physx::PxGeometryHolder CreateGeometry() = 0;
	void ResetShape();

public:
	// 현재 시뮬레이션 상황 :
	// shape를 attach하기 전엔 사용 불가. 그래서 현재 Init에는 rigidBody를 인자로 들고 온다.

	// value 0~31
	uint8_t GetLayerIndex() const;
	void SetLayerIndex(uint8_t layerIndex);

	RigidBody* GetRigidBody() const;
	physx::PxShape* GetPxShape() const;

	float GetFriction(float value) const;
	void SetFriction(float value);

	//FrictionFlag
	PhysicsCombineMode GetFrictionCombineMode() const;
	void SetFrictionCombineMode(PhysicsCombineMode value);
	//RestitutionFlag
	PhysicsCombineMode GetRestitutionCombineMode() const;
	void SetRestitutionCombineMode(PhysicsCombineMode value);
	void SetMaterialFlag(physx::PxMaterialFlag::Enum, bool value);
	void SetID(int32_t id);

public:
	void CollectCollisionInfo(CollisionInfoType type, std::shared_ptr<CollisionPairInfo> info);
	void CollectTriggerInfo(CollisionInfoType type, std::shared_ptr<TriggerPairInfo> info);
	void ClearCollisionInfo();
	const std::vector<std::shared_ptr<CollisionPairInfo>>& GetCollisionInfo(CollisionInfoType type) const;

public:
	void SetTrigger(bool value);
	bool IsTrigger();

protected:
	RigidBody* m_attachedRigidBody = nullptr;
	physx::PxMaterial* m_material = nullptr;
	physx::PxShape* m_shape = nullptr;

	bool m_isTrigger = false;
	uint8_t m_layerIndex = 0;
	uint32_t m_ignoreLayerBits = 0x00000000;

	int32_t m_id;

public:
	std::vector<std::shared_ptr<CollisionPairInfo>> m_CollisionEnter;
	std::vector<std::shared_ptr<CollisionPairInfo>> m_CollisionExit;
	std::vector<std::shared_ptr<CollisionPairInfo>> m_CollisionStay;

	std::vector<std::shared_ptr<TriggerPairInfo>> m_TriggerEnter;
	std::vector<std::shared_ptr<TriggerPairInfo>> m_TriggerExit;
};


