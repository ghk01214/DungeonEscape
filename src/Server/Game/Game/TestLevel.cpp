#include "pch.h"
#include "TestLevel.h"
#include "ObjectManager.h"
#include "Player.h"
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

	//auto PlayerObject = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 3, Vec3(150, 200, -150), Quat(0, 0, 0, 1), Vec3(50, 50, 50));

#pragma region Sphere
	//auto SphereObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(5, 10, 5), Quat(0, 0, 0, 1), Vec3(2,2,2));
	//auto SphereBody = SphereObject->GetComponent<RigidBody>(L"RigidBody");
	//SphereBody->SetKinematic(false);
	//SphereBody->SetCCDFlag(true);
	//SphereBody->SetRigidBodySleep(false);
	//SphereBody->AddCollider<SphereCollider>(SphereObject->GetTransform()->GetScale());

	//SphereBody->SetAngularDamping(0.00001f);
	//SphereBody->SetLinearDamping(0.15f);
	//SphereBody->SetMass(SphereBody->GetMass() * 0.20f);
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
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\CUBE.fbx");


	// actor 정보 로드
	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\Map\\Stage2.FBX");

	auto& mapInfo = mapLoader.GetMapObjectInfo();
	for (auto& info : mapInfo)
	{
		const objectLocationInfo& locationInfo = info.second;
		auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map",
			Vec3(locationInfo.Position.x , locationInfo.Position.y , locationInfo.Position.z ),
			Quat::FromEuler(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z),
			Vec3(locationInfo.Scale.x, locationInfo.Scale.y, locationInfo.Scale.z)
		);

		auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
		auto& vertexindexInfo = mapLoader.FindVertexIndicesInfo(info.first);
		MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), info.first, vertexindexInfo);
	}
#pragma endregion

#pragma region CenterBox(0,0,0)
	auto centerObj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 0, 0), Quat(0, 0, 0, 1), Vec3(100, 100, 100));
	auto centerBody = centerObj->GetComponent<RigidBody>(L"RigidBody");
	centerBody->AddCollider<BoxCollider>(centerObj->GetTransform()->GetScale());
#pragma endregion

#pragma region Z100Box(1000,0,0)
	auto Box1Obj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 0, 1000), Quat(0, 0, 0, 1), Vec3(300, 300, 300));
	auto Box1Body = Box1Obj->GetComponent<RigidBody>(L"RigidBody");
	Box1Body->AddCollider<BoxCollider>(Box1Obj->GetTransform()->GetScale());
#pragma endregion

#pragma region Plane
	//auto MapPlaneObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 0, 0), Quat(0, 0, 0, 1), Vec3(1000, 2, 1000));
	//auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
	//MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());
#pragma endregion


}

void TestLevel::Update(double timeDelta)
{
	game::MessageHandler::GetInstance()->ExecuteMessage();

	//if (GetAsyncKeyState('P') & 0x8000)
	//{	
	//	auto objMgr = ObjectManager::GetInstance();
	//	auto playerlist = objMgr->GetLayer(L"Layer_Player")->GetGameObjects();
	//	for (auto& p : playerlist)
	//		p->GetComponent<RigidBody>(L"RigidBody")->SetPosition(Vec3(0, 1000, 0), false);
	//}
}

void TestLevel::LateUpdate(double timeDelta)
{
}

void TestLevel::Release(void)
{
}