#pragma once
#include "physx_utils.h"

enum class ColliderShape;

class RigidBody;
class CustomSimulationEventCallback;
class CustomFilterShader;
class ControllerManagerWrapper;
class PhysQuery;
class CustomController;

class PhysDevice
{
public:
	DeclareSingletone(PhysDevice);

private:
	PhysDevice();
	~PhysDevice();

public:
	void Init();
	void StepSim();
	void Release();


public:
	physx::PxPhysics* GetPhysics() const;
	physx::PxScene* GetScene() const;
	PhysQuery* GetQuery() const;
	physx::PxCooking* GetCooking() const;
	physx::PxMaterial* GetDefaultMaterial() const;
	ControllerManagerWrapper* GetControllerManagerWrapper() const;

public:
	//Test용 함수
	void CreateHelloWorldStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent, bool attributeStatic);
	void CreateHelloWorldBox(bool attributeStatic);
	void CreateHelloWorldDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry);
	void InitialPlacement();

	RigidBody* CreateDynamic(ColliderShape shape, float posX, float posY, float posZ);

	void SetLinearVelocity();			//가속도 설정
	void SetGlobalPoseRotation();		//각도 변경
	void AddForce();					//힘 적용

	void GameLogic();
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

	std::vector<RigidBody*>					m_RigidBodies;
	CustomController*						m_customController;
};

