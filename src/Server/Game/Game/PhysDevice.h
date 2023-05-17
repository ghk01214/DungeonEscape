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
	physx::PxCooking*		GetCooking() const;

	PhysQuery*				GetQuery() const;
	CustomSimulationEventCallback* GetEventCallback();

private:
	physx::PxDefaultAllocator				m_Allocator;
	physx::PxDefaultErrorCallback			m_ErrorCallback;

	physx::PxFoundation*					m_Foundation	= nullptr;
	physx::PxPhysics*						m_Physics		= nullptr;
	physx::PxDefaultCpuDispatcher*			m_Dispatcher	= nullptr;
	physx::PxScene*							m_Scene			= nullptr;
	physx::PxCooking*						m_Cooking		= nullptr;
	physx::PxPvd*							m_Pvd			= nullptr;

	CustomSimulationEventCallback*			m_eventCallback = nullptr;
	CustomFilterShader*						m_filterShader	= nullptr;
	PhysQuery*								m_query			= nullptr;
};

