#include "pch.h"
#include "TestLevel.h"
#include "ObjectManager.h"
#include "UnitObject.h"
#include "MapObject.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "Transform.h"
#include "GameObject.h"
#include "CustomController.h"
#include "Component.h"
#include "MessageHandler.h"
#include "FBXMapLoader.h"

TestLevel::TestLevel()
{
}

TestLevel::~TestLevel()
{
}

void TestLevel::Init()
{
	auto objmgr = ObjectManager::GetInstance();
	objmgr->AddLayer(L"Layer_Map");
	objmgr->AddLayer(L"Layer_Player");

	//messagehandler->executeOrder()
	//UnitObject* player{ objmgr->AddGameObjectToLayer<UnitObject>(L"Layer_Player", Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5)) };
	//auto PlayerObject = objmgr->AddGameObjectToLayer<UnitObject>(L"Layer_Player", 3, Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5));

#pragma region Plane
	//auto MapPlaneObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 2, 0), Quat(0, 0, 0, 1), Vec3(100, 2, 100));
	//auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
	//MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());
#pragma endregion

#pragma region Sphere
	auto SphereObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(5, 10, 5), Quat(0, 0, 0, 1), Vec3(2,2,2));
	auto SphereBody = SphereObject->GetComponent<RigidBody>(L"RigidBody");
	SphereBody->SetKinematic(false);
	SphereBody->SetCCDFlag(true);
	SphereBody->SetRigidBodySleep(false);
	SphereBody->AddCollider<SphereCollider>(SphereObject->GetTransform()->GetScale());
	
	SphereBody->SetAngularDamping(0.00001f);
	SphereBody->SetLinearDamping(0.15f);
	SphereBody->SetMass(SphereBody->GetMass() * 0.20f);
#pragma endregion

#pragma region Box1
	//auto Box1Object = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(20, 10, 0), Quat(0, 0, 0, 1), Vec3(15, 15, 15));
	//auto Box1Body = Box1Object->GetComponent<RigidBody>(L"RigidBody");
	//Box1Body->AddCollider<BoxCollider>(Box1Object->GetTransform()->GetScale());
	//Box1Body->SetRotation(45.f, PhysicsAxis::Y);
#pragma endregion

#pragma region Box2
	//auto Box2Object = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(10, 5.5, 0), Quat(0, 0, 0, 1), Vec3(10, 1, 10));
	//auto Box2Body = Box2Object->GetComponent<RigidBody>(L"RigidBody");
	//Box2Body->AddCollider<BoxCollider>(Box2Object->GetTransform()->GetScale());
	//Box2Body->SetRotation(-45.f, PhysicsAxis::X);
#pragma endregion

#pragma region Mesh
	//auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(-10, 25, 0), Quat(0, 0, 0, 1), Vec3(1, 1, 1));
	//auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
	//MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), L"Moon", L"..\\Resources\\FBX\\Moon\\moon.fbx");
#pragma endregion

#pragma region MAP
	// static Mesh 정보 로드

	/*
		1. static Mesh 오브젝트를 로드한다. -> 로드해서 어디 넣지?
		2. MeshCollider가 가지고 있는 static ConvexMeshWrapper 변수에 staticMesh 오브젝트의 정보를 넣는다.
		3. 맵 fbx 파일을 로드한다.
		4. 로드한 맵 정보로부터 actor가 사용하는 staticMesh 오브젝트의 정보와 위치를 받아 맵 오브젝트를 생성한다.
		5. 생성된 맵 오브젝트는 오브젝트 매니저에 넣는다.
	*/

	FBXMapLoader mapLoader;

	// static Mesh 정보 로드
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Bones.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Floors.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Misc.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Optimized.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Pillars.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Rocks.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Walls.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Wood.fbx");


	// actor 정보 로드
	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\Map\\StageTest(ascii).FBX");

	auto& mapInfo = mapLoader.GetMapObjectInfo();
	for (auto& info : mapInfo)
	{
		const objectLocationInfo& locationInfo = info.second;

		auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map",
			Vec3(locationInfo.Position.x * 0.1f, locationInfo.Position.y * 0.1f, locationInfo.Position.z * 0.1f),
			Quat(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z, 1),
			Vec3(locationInfo.Scale.x * 0.01f, locationInfo.Scale.y * 0.01f, locationInfo.Scale.z * 0.01f)
		);

		auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
		auto& vertexindexInfo = mapLoader.FindVertexIndicesInfo(info.first);
		MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), info.first, vertexindexInfo);
	}
#pragma endregion
}

void TestLevel::Update(double timeDelta)
{
	game::MessageHandler::GetInstance()->ExecuteMessage();
}

void TestLevel::LateUpdate(double timeDelta)
{
}

void TestLevel::Release(void)
{
}