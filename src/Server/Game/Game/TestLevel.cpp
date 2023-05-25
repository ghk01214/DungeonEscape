#include "pch.h"
#include "TestLevel.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Monster.h"
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
	objmgr->AddLayer(L"Layer_Monster");
	objmgr->AddLayer(L"Layer_Map2");
	objmgr->AddLayer(L"Layer_SkillObject");


	//auto PlayerObject = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 1, Vec3(1500, 100, -1500), Quat(0, 0, 0, 1), Vec3(50, 50, 50));

	auto MonsterObject = objmgr->AddGameObjectToLayer<Monster>(L"Layer_Monster", game::MessageHandler::GetInstance()->NewObjectID(), Vec3(1500, 200, -1000), Quat(0, 0, 0, 1), Vec3(50, 50, 50));
	MonsterObject->SetName(L"Dragon");
	MonsterObject->SetObjectType(server::OBJECT_TYPE::BOSS);
	MonsterObject->SetFBXType(server::FBX_TYPE::DRAGON);
	MonsterObject->GetController()->GetCollider()->SetID(game::MessageHandler::GetInstance()->NewColliderID());

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
	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\Map\\Stage4.FBX");

	auto& mapInfo = mapLoader.GetMapObjectInfo();
	int32_t i = 0;
	for (auto& info : mapInfo)
	{
		switch (i)
		{
			case 944: case 945: case 1068:
			case 1069: case 1070: case 1071:
			++i;
			continue;
			break;
		}

		const objectLocationInfo& locationInfo = info.second;
		auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map",
			Vec3(locationInfo.Position.x * PX_SCALE_FACTOR, locationInfo.Position.y * PX_SCALE_FACTOR, locationInfo.Position.z * PX_SCALE_FACTOR),
			Quat::FromEuler(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z),
			Vec3(locationInfo.Scale.x, locationInfo.Scale.y, locationInfo.Scale.z)
		);

		auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
		auto& vertexindexInfo = mapLoader.FindVertexIndicesInfo(info.first);
		MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), info.first, vertexindexInfo);

		++i;
	}

	std::system("cls");

	std::cout << "Map loading finished\n";
#pragma endregion

#pragma region CenterBox(0,0,0)
	//	auto centerObj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(0, 0, 0), Quat(0, 0, 0, 1), Vec3(100, 1000, 100));
	//	auto centerBody = centerObj->GetComponent<RigidBody>(L"RigidBody");
	//	centerBody->AddCollider<BoxCollider>(centerObj->GetTransform()->GetScale());
	//#pragma endregion
	//
	//#pragma region Z100Box(1000,0,0)
	//	auto Box1Obj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(0, 0, 1000), Quat(0, 0, 0, 1), Vec3(300, 300, 300));
	//	auto Box1Body = Box1Obj->GetComponent<RigidBody>(L"RigidBody");
	//	Box1Body->AddCollider<BoxCollider>(Box1Obj->GetTransform()->GetScale());
	//#pragma endregion
	//
	//#pragma region Plane
	//	auto MapPlaneObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(0, 0, 0), Quat(0, 0, 0, 1), Vec3(5000, 2, 5000));
	//	auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
	//	MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());
#pragma endregion


}

void TestLevel::Update(double timeDelta)
{
	game::MessageHandler::GetInstance()->ExecuteMessage();

	//P K I로 맵 오브젝트 삭제(removalFlag함수 사용후 삭제작업 처리하면 됨), 몬스터 패턴 실행, 플레이어 공 발사 등을 실험 가능
	//한번 실행했으면 O를 누르면 다시 명령어 실행가능

	 //맵 오브젝트 삭제
	//if (GetAsyncKeyState('P') & 0x8000)
	//{
	//	auto objMgr = ObjectManager::GetInstance();
	//	auto map2 = objMgr->GetLayer(L"Layer_Map2")->GetGameObjects();
	//	(*map2.begin())->SetRemoveReserved();
	//	std::cout << "맵 오브젝트 삭제" << std::endl;
	//}

	//static bool once = true;
	////몬스터 패턴 테스트

	//if (GetAsyncKeyState('I') & 0x8000)
	//{
	//	if (once)
	//	{
	//		auto objMgr = ObjectManager::GetInstance();
	//		auto players = objMgr->GetLayer(L"Layer_Player")->GetGameObjects();
	//		for (auto& p : players)
	//			dynamic_cast<Player*>(p)->PlayerPattern_ShootBall(100.f);
	//		once = false;
	//	}
	//}

	//if (GetAsyncKeyState('O') & 0x8000)
	//	once = true;

	//using namespace std::chrono;
	//static steady_clock::time_point bossSkillEndTime{ steady_clock::now() };
	//static bool prevOnGround{ true };

	//auto objMgr = ObjectManager::GetInstance();
	//auto monsters = objMgr->GetLayer(L"Layer_Monster")->GetGameObjects();

	//for (auto& monster : monsters)
	//{
	//	auto mob{ dynamic_cast<Monster*>(monster) };

	//	if (prevOnGround != mob->IsOnGround())
	//	{
	//		//mob->MonsterPattern_GroundHit();

	//		bossSkillEndTime = steady_clock::now();
	//	}

	//	prevOnGround = mob->IsOnGround();
	//}

	//if (steady_clock::now() - bossSkillEndTime > 5s)
	//{
	//	for (auto& monster : monsters)
	//	{
	//		auto mob{ dynamic_cast<Monster*>(monster) };

	//		if (mob->IsDead() == false)
	//		{
	//			mob->GetController()->SetSpaceKeyDown(true);

	//			game::Message msg{ -1, ProtocolID::WR_JUMP_START_ACK };
	//			msg.objID = mob->GetMonsterID();
	//			game::MessageHandler::GetInstance()->PushSendMessage(msg);
	//		}
	//	}
	//}
}

void TestLevel::LateUpdate(double timeDelta)
{
}

void TestLevel::Release(void)
{
}