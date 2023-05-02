#include "pch.h"
#include "PhysDevice.h"
#include "CustomFilterShader.h"
#include "CustomSimulationEventCallback.h"
#include "RigidBody.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "CustomController.h"
#include "ControllerManagerWrapper.h"
#include "ControllerWrapper.h"
#include "PhysQuery.h"
#include "CollisionPairInfo.h"

#include "ObjectManager.h"
#include "Layer.h"
#include "GameObject.h"

using namespace physx;

ImplementSingletone(PhysDevice);

PhysDevice::PhysDevice()
{
}

PhysDevice::~PhysDevice()
{
	Release();
}

void PhysDevice::Init()
{
	m_eventCallback = new CustomSimulationEventCallback;
	m_filterShader = new CustomFilterShader;

	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	m_Pvd = PxCreatePvd(*m_Foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true, m_Pvd);

	PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_Dispatcher;


	//sceneDesc.filterShader = PxDefaultSimulationFilterShader
	sceneDesc.filterShader = CustomFilterShader::PxDefaultSimulationFilterShader;
	sceneDesc.filterCallback = m_filterShader;
	sceneDesc.simulationEventCallback = m_eventCallback;

	m_Scene = m_Physics->createScene(sceneDesc);

#pragma region pvd

	PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

#pragma endregion pvd

	PxTolerancesScale scale;
	PxCookingParams cookingParams(scale);
	cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
	cookingParams.meshWeldTolerance = 0.001f;

	m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, cookingParams);

	m_query = new PhysQuery;
	m_query->Init();

#pragma region error searching
	//PxMaterial* mat = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	//PxRigidStatic * groundPlane = PxCreatePlane(*m_Physics, PxPlane(0, 1, 0, 0), *mat);
	//m_Scene->addActor(*groundPlane);

	//PxRigidDynamic* testDynamic = m_Physics->createRigidDynamic(PxTransform(0, 10, 0));

	//PxShape* shape = m_Physics->createShape(PxBoxGeometry(5.0f, 5.0f, 5.0f), *mat);
	//testDynamic->attachShape(*shape);
	//m_Scene->addActor(*testDynamic);
	//testDynamic->wakeUp();
#pragma endregion
}

void PhysDevice::StepSim(double timeDelta)
{
	m_Scene->simulate(1.0f / PX_SIM_FRAMECNT);
	m_Scene->fetchResults(true);
}

void PhysDevice::PreUpdate()
{
	m_eventCallback->Notify();
}

void PhysDevice::Update(double timeDelta)
{
	ClearEventCallback();
	StepSim(timeDelta);
}

void PhysDevice::LateUpdate(double timeDelta)
{

}

void PhysDevice::Release()
{
	// 1. custom objects
	SafeDelete(m_query);
	SafeDelete(m_eventCallback);
	SafeDelete(m_filterShader);

	// 2. physx objects (in order)
	PX_RELEASE(m_Scene);          // Scene should be released before dispatcher, physics, and foundation
	PX_RELEASE(m_Dispatcher);     // Dispatcher should be released before physics and foundation
	PX_RELEASE(m_Physics);        // Physics should be released before foundation
	PX_RELEASE(m_Cooking);        // Cooking should be released before foundation

	// 3. PVD, attached transport
	if (m_Pvd)
	{
		PxPvdTransport* transport = m_Pvd->getTransport();
		m_Pvd->release();          // Release the PVD object before releasing the transport and foundation
		m_Pvd = nullptr;
		PX_RELEASE(transport);     // Release the transport before releasing the foundation
	}

	// 4. Release the foundation object last
	PX_RELEASE(m_Foundation);

	printf("PhysDevice Release done.\n");
}

PxPhysics* PhysDevice::GetPhysics() const
{
	return m_Physics;
}

PxScene* PhysDevice::GetScene() const
{
	return m_Scene;
}

PhysQuery* PhysDevice::GetQuery() const
{
	return m_query;
}

PxCooking* PhysDevice::GetCooking() const
{
	return m_Cooking;
}

void PhysDevice::ClearEventCallback()
{	
	////모든 게임오브젝트의 리지드 바디 ClearCollidersCollisionInfo 호출
	//const auto& objmgr = ObjectManager::GetInstance();
	//auto& layers = objmgr->GetLayers();
	//for (const auto& layer : layers)
	//{
	//	auto& objectList = layer.second->GetGameObjects();
	//	for (auto& object : objectList)
	//	{
	//		RigidBody* body = object->GetComponent<RigidBody>(L"RigidBody");
	//		if (body)
	//		{
	//			body->ClearCollidersCollisionInfo();
	//		}
	//		else
	//		{
	//			CustomController* controller = object->GetComponent<CustomController>(L"CustomController");
	//			if (controller)
	//			{
	//				controller->ClearControllerCollisionInfo();
	//			}
	//		}
	//	}
	//}

	m_eventCallback->ClearVector();
}






