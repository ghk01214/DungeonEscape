#pragma once
#include "physx_define.h"

class RigidBody;
class CustomSimulationEventCallback;
class CustomFilterShader;
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
	void PreUpdate();								
	void StepSim(double timeDelta);
	void ClearEventCallback();						//순서를 반드시 지켜주세요.
	void Update(double timeDelta);					//PhysDevice::PreUpdate() > ObjMgr::Update() > PhysDevice::Update() > PhysDevice::Late_Update()
	void LateUpdate(double timeDelta);
	void Release();
public:
	physx::PxPhysics*		GetPhysics() const;
	physx::PxScene*			GetScene() const;
	PhysQuery*				GetQuery() const;
	physx::PxCooking*		GetCooking() const;
	physx::PxMaterial*		GetDefaultMaterial() const;

public:
	//Test용 함수
	void CreateHelloWorldStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent, bool attributeStatic);
	void InitialPlacement();

	void TestCreateDynamic(ColliderShape shape, float posX, float posY, float posZ);
	//CustomController* CreateCustomController(physx::PxVec3 pos);	
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
	PhysQuery*								m_query = nullptr;
};

