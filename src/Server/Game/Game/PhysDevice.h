#pragma once
#include "physx_define.h"

class RigidBody;
class CustomSimulationEventCallback;
class CustomFilterShader;
class ControllerManagerWrapper;
class PhysQuery;
class CustomController;

class PhysDevice
{
public:
	DeclareSingletone(PhysDevice)

private:
	PhysDevice();
	~PhysDevice();

public:
	void Init();
	void StepSim();
	void GameLogic();
	void ClearEventCallback();
	void PreUpdate();				//순서를 반드시 지켜주세요.
	void Update();					//PhysDevice::PreUpdate() > CompMgr::Update() > PhysDevice::Update() > PhysDevice::Late_Update()
	void LateUpdate();		
	void Release();
public:
	physx::PxPhysics*		GetPhysics() const;
	physx::PxScene*			GetScene() const;
	PhysQuery*				GetQuery() const;
	physx::PxCooking*		GetCooking() const;
	physx::PxMaterial*		GetDefaultMaterial() const;
	ControllerManagerWrapper* GetControllerManagerWrapper() const;

public:
	//Test용 함수
	void CreateHelloWorldStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent, bool attributeStatic);
	void InitialPlacement();

	RigidBody* CreateDynamic(ColliderShape shape, float posX, float posY, float posZ);
	CustomController* CreateCustomController(physx::PxVec3 pos);

	void SetLinearVelocity(int containerIdx, physx::PxVec3 linearVel);			
	void SetGlobalPoseRotation(int containerIdx, PhysicsAxis axis, float degree);			
private:
	physx::PxDefaultAllocator				m_Allocator;
	physx::PxDefaultErrorCallback			m_ErrorCallback;

	physx::PxFoundation*					m_Foundation = NULL;
	physx::PxPhysics*						m_Physics = NULL;

	physx::PxDefaultCpuDispatcher*			m_Dispatcher = NULL;
	physx::PxScene*							m_Scene = NULL;

	physx::PxCooking*						m_cooking = NULL;

	physx::PxMaterial*						m_Material = NULL;

	physx::PxPvd*							m_Pvd = NULL;

	CustomSimulationEventCallback*			m_eventCallback = nullptr;
	CustomFilterShader*						m_filterShader = nullptr;
	ControllerManagerWrapper*				m_controllerManagerWrapper = nullptr;
	PhysQuery*								m_query = nullptr;

	std::vector<std::shared_ptr<RigidBody>>				m_RigidBodies;
	std::vector<std::shared_ptr<CustomController>>		m_customControllers;
};

