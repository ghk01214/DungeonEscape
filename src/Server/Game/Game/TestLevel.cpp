#include "pch.h"
#include "TestLevel.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Monsters.hpp"
#include "MapObject.h"
#include "PillarObject.h"
#include "TriggerObject.h"
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
#include "TriggerObject2.h"
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
	objmgr->AddLayer(L"Layer_Gimmik_Boulder");
	objmgr->AddLayer(L"Layer_Gimmik_Rock");
	objmgr->AddLayer(L"Layer_Gimmik_Pillar");
	objmgr->AddLayer(L"Layer_Player");
	objmgr->AddLayer(L"Layer_Monster");
	objmgr->AddLayer(L"Layer_Map2");
	objmgr->AddLayer(L"Layer_SkillObject");
	objmgr->AddLayer(L"Layer_TriggerObject");

	//LoadBasicMap4();

	//LoadUnit_DebugMode();
	LoadMap();
}

void TestLevel::Update(double timeDelta)
{
	game::MessageHandler::GetInstance()->ExecuteMessage();

	static bool boulderSummoned = false;
	if (!boulderSummoned)
	{
		bool inRange = ThrowGimmik2Ball_RangeCheck();

		if (inRange)
		{
			ThrowGimmik2Ball();
			boulderSummoned = true;
		}
	}

	static bool meetMonster = false;
	if (meetMonster == false)
	{
		auto players{ ObjectManager::GetInstance()->GetLayer(L"Layer_Player")->GetGameObjects() };

		for (auto& player : players)
		{
			auto pl{ dynamic_cast<Player*>(player) };

			if (pl == nullptr)
				continue;

			if (pl->GetTransform()->GetPosition().z > 40490.f)
			{
				game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_SOUND_ACK };
				ev.objID = pl->GetID();
				ev.state = magic_enum::enum_integer(server::SOUND_TYPE::BATTLE);

				game::MessageHandler::GetInstance()->PushSendMessage(ev);

				meetMonster = true;
				break;
			}
		}
	}
}

void TestLevel::LateUpdate(double timeDelta)
{
}

void TestLevel::Release(void)
{
}

void TestLevel::LoadUnit_DebugMode()
{
	auto objmgr = ObjectManager::GetInstance();

	//auto PlayerObject = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 1, Vec3(16229, -3541, 35577), Quat(0, 0, 0, 1), Vec3(75, 75, 75));
	//auto PlayerObject2 = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 2, Vec3(13060.874, -3541.767, 37596.410), Quat(0, 0, 0, 1), Vec3(75, 75, 75));
																																							//골렘 입장위치		16229, -3541, 35577
																																							//위퍼 입장위치		0, -720, 7500
																																							//다리				3890, -1462, 21062
																																							//돌테스트			15609, -976, 26457
	//15609, -976, 26457
	auto GolemObject = objmgr->AddGameObjectToLayer<Golem>(L"Layer_Monster", 4, Vec3(16200, -3000, 45000), Quat(0, 0, 0, 1), Vec3(150, 150, 150));

	//auto WeeperObject = objmgr->AddGameObjectToLayer<Weeper>(L"Layer_Monster", 4, Vec3(0, -720, 11060), Quat(0, 0, 0, 1), Vec3(150, 150, 150));

	//몬스터
	//위퍼 위치15037.582, -1550.337, 21630.693; 0.000, 0.000, 0.707, 0.707;
	//골렘 16220, -3700, 45890
}

void TestLevel::LoadMap()
{
#pragma region 안내
/*static Mesh 정보 로드
	1. static Mesh 오브젝트를 로드한다. -> 로드해서 어디 넣지?
	2. MeshCollider가 가지고 있는 static ConvexMeshWrapper 변수에 staticMesh 오브젝트의 정보를 넣는다.
	3. 맵 fbx 파일을 로드한다.
	4. 로드한 맵 정보로부터 actor가 사용하는 staticMesh 오브젝트의 정보와 위치를 받아 맵 오브젝트를 생성한다.
	5. 생성된 맵 오브젝트는 오브젝트 매니저에 넣는다.
*/
#pragma endregion

	bool debug = false;
	auto objmgr = ObjectManager::GetInstance();

	if (debug)
	{
		LoadDebugMap_Bridge();
		//LoadDebugMap_Boulder();
		LoadGimmikObject();
	}
	else
	{
		LoadMapObject();
		//LoadPotObject();
		LoadGimmikObject();
		LoadTriggerObject();
	}

	std::system("cls");
	std::cout << "Map loading finished\n";
}

void TestLevel::LoadBasicMap1()
{
	auto objmgr = ObjectManager::GetInstance();

#pragma region Sphere
	auto SphereObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(5, 10, 5), Quat(0, 0, 0, 1), Vec3(2, 2, 2));
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
	auto Box1Object = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(20, 10, 0), Quat(0, 0, 0, 1), Vec3(15, 15, 15));
	auto Box1Body = Box1Object->GetComponent<RigidBody>(L"RigidBody");
	Box1Body->AddCollider<BoxCollider>(Box1Object->GetTransform()->GetScale());
	Box1Body->SetRotation(45.f, PhysicsAxis::Y);
#pragma endregion

#pragma region Box2
	auto Box2Object = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(10, 5.5, 0), Quat(0, 0, 0, 1), Vec3(10, 1, 10));
	auto Box2Body = Box2Object->GetComponent<RigidBody>(L"RigidBody");
	Box2Body->AddCollider<BoxCollider>(Box2Object->GetTransform()->GetScale());
	Box2Body->SetRotation(-45.f, PhysicsAxis::X);
#pragma endregion
}

void TestLevel::LoadBasicMap2()
{
	auto objmgr = ObjectManager::GetInstance();
	//auto PlayerObject = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 1, Vec3(1500, 100, -1500), Quat(0, 0, 0, 1), Vec3(50, 50, 50));
	//PlayerObject->SetControllerPosition(Vec3(150.f, 100.f, 0.f));


#pragma region CenterBox(0,0,0)
	auto centerObj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(0, 0, 0), Quat(0, 0, 0, 1), Vec3(100, 1000, 100));
	auto centerBody = centerObj->GetComponent<RigidBody>(L"RigidBody");
	centerBody->AddCollider<BoxCollider>(centerObj->GetTransform()->GetScale());
#pragma endregion

#pragma region Z100Box(1000,0,0)
	auto Box1Obj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(0, 0, 1000), Quat(0, 0, 0, 1), Vec3(300, 300, 300));
	auto Box1Body = Box1Obj->GetComponent<RigidBody>(L"RigidBody");
	Box1Body->AddCollider<BoxCollider>(Box1Obj->GetTransform()->GetScale());
#pragma endregion

#pragma region TriggerBox(300,0,0)
	auto TriggerObj = objmgr->AddGameObjectToLayer<TriggerObject>(L"Trigger", Vec3(300, 100, 0), Quat(0, 0, 0, 1), Vec3(150, 100, 100));
	TriggerObj->SetTriggerType(server::TRIGGER_TYPE::SINGLE_STRIKE);
	auto TriggerBody = TriggerObj->GetComponent<RigidBody>(L"RigidBody");
	TriggerBody->AddCollider<BoxCollider>(TriggerObj->GetTransform()->GetScale());
	TriggerBody->GetCollider(0)->SetTrigger(true);
#pragma endregion

#pragma region Plane
	auto MapPlaneObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(0, 0, 0), Quat(0, 0, 0, 1), Vec3(5000, 2, 5000));
	auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
	MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());
#pragma endregion
}

void TestLevel::LoadBasicMap3()
{
	auto objmgr = ObjectManager::GetInstance();
	auto PlayerObject = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 1, Vec3(-300.f, 1000.f, 0.f), Quat(0, 0, 0, 1), Vec3(50, 50, 50));
	auto PlayerObject2 = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 2, Vec3(-300.f, 1000.f, 300.f), Quat(0, 0, 0, 1), Vec3(50, 50, 50));
	auto WeeperObject = objmgr->AddGameObjectToLayer<Weeper>(L"Layer_Monster", 3, Vec3(1050.f, 100.f, 0.f), Quat(0, 0, 0, 1), Vec3(100, 100, 100));
	//auto GolemObject = objmgr->AddGameObjectToLayer<Golem>(L"Layer_Monster", 3, Vec3(1050.f, 100.f, 0.f), Quat(0, 0, 0, 1), Vec3(100, 100, 100));


#pragma region CenterBox(0,0,0)
	//auto centerObj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(750, 600, 0), Quat(0, 0, 0, 1), Vec3(100, 100, 100));
	//auto centerBody = centerObj->GetComponent<RigidBody>(L"RigidBody");
	//centerBody->AddCollider<BoxCollider>(centerObj->GetTransform()->GetScale());
	//centerObj->ApplyRequestedLayers();
#pragma endregion

#pragma region Box(1000,0,0)
	auto Box1Obj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(200, 0, -400), Quat(0, 0, 0, 1), Vec3(300, 500, 300));
	auto Box1Body = Box1Obj->GetComponent<RigidBody>(L"RigidBody");
	Box1Body->AddCollider<BoxCollider>(Box1Obj->GetTransform()->GetScale());
	Box1Obj->ApplyRequestedLayers();
#pragma endregion


#pragma region Plane
	auto MapPlaneObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(750, 0, 0), Quat(0, 0, 0, 1), Vec3(5000, 2, 5000));
	auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
	MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());
	MapPlaneObject->ApplyRequestedLayers();
#pragma endregion
}

void TestLevel::LoadBasicMap4()
{
	auto objmgr = ObjectManager::GetInstance();

	auto PlayerObject = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 1, Vec3(0, 200, -42), Quat(0, 0, 0, 1), Vec3(75,75,75));

	auto MapPlaneObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map2", Vec3(0, 0, -42), Quat(0, 0, 0, 1), Vec3(9000, 2, 9000));
	auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
	MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());
	MapPlaneObject->ApplyRequestedLayers();


	//1회성 trigger2 샘플
	auto Box1Obj = objmgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(0, 300, 500), Quat(0, 0, 0, 1), Vec3(100, 100, 100), true);
	auto Box1Body = Box1Obj->GetComponent<RigidBody>(L"RigidBody");
	Box1Body->AddCollider<BoxCollider>(Box1Obj->GetTransform()->GetScale());

	//다회성 trigger2 샘플
	auto Box2Obj = objmgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(-200, 300, 500), Quat(0, 0, 0, 1), Vec3(100, 100, 100), false);
	auto Box2Body = Box2Obj->GetComponent<RigidBody>(L"RigidBody");
	Box2Body->AddCollider<BoxCollider>(Box2Obj->GetTransform()->GetScale());



	//포탈1
	auto Box3Obj = objmgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(-400, 300, 500), Quat(0, 0, 0, 1), Vec3(100, 100, 100), false);
	auto Box3Body = Box3Obj->GetComponent<RigidBody>(L"RigidBody");
	Box3Body->AddCollider<BoxCollider>(Box3Obj->GetTransform()->GetScale());
	Box3Obj->SetTriggerAttribute(TriggerObject2::TRIGGERATTRIBUTE::PORTAL1);

	//포탈2
	auto Box4Obj = objmgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(-600, 300, 500), Quat(0, 0, 0, 1), Vec3(100, 100, 100), false);
	auto Box4Body = Box4Obj->GetComponent<RigidBody>(L"RigidBody");
	Box4Body->AddCollider<BoxCollider>(Box4Obj->GetTransform()->GetScale());
	Box4Obj->SetTriggerAttribute(TriggerObject2::TRIGGERATTRIBUTE::PORTAL2);
}

void TestLevel::LoadMapObject()
{
	auto objmgr = ObjectManager::GetInstance();
	FBXMapLoader mapLoader;
	//mapLoader.SetDebugMode(true);

	mapLoader.AddBasicObject(L"..\\..\\..\\Client\\Resources\\FBX\\Models\\Models.fbx");	// Mesh 로드
	mapLoader.AddBasicObject(L"..\\..\\..\\Client\\Resources\\FBX\\Models\\Models2.fbx");
	mapLoader.AddBasicObject(L"..\\..\\..\\Client\\Resources\\FBX\\Models\\GimmicksRAW.fbx");
	mapLoader.ExtractMapInfo(L"..\\..\\..\\Client\\Resources\\FBX\\Server.fbx");			// Map 로드

	auto& mapInfo = mapLoader.GetMapObjectInfo();
	for (auto& info : mapInfo)
	{
		const objectLocationInfo& locationInfo = info.second;

		auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map",
		   Vec3(locationInfo.Position.x * PX_SCALE_FACTOR, locationInfo.Position.y * PX_SCALE_FACTOR, locationInfo.Position.z * PX_SCALE_FACTOR),
		   Quat::FromEuler(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z),
		   Vec3(locationInfo.Scale.x, locationInfo.Scale.y, locationInfo.Scale.z)
		);

		auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
		auto& vertexindexInfo = mapLoader.FindVertexIndicesInfo(info.first);
		MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), info.first, vertexindexInfo);
		MeshObject->ApplyRequestedLayers();
	}
}

void TestLevel::LoadPotObject()
{
	auto objmgr = ObjectManager::GetInstance();
	FBXMapLoader potLoader;

	potLoader.AddBasicObject(L"..\\..\\..\\Client\\Resources\\FBX\\Models\\MapObjectsRAW.fbx");		// Mesh 로드
	potLoader.ExtractMapInfo(L"..\\..\\..\\Client\\Resources\\FBX\\MapObjects.fbx");				// Pot 정보 로드

	auto& potInfo = potLoader.GetMapObjectInfo();
	for (auto& info : potInfo)
	{
		const objectLocationInfo& locationInfo = info.second;

		auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map",
		   Vec3(locationInfo.Position.x * PX_SCALE_FACTOR, locationInfo.Position.y * PX_SCALE_FACTOR, locationInfo.Position.z * PX_SCALE_FACTOR),
		   Quat::FromEuler(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z),
		   Vec3(locationInfo.Scale.x, locationInfo.Scale.y, locationInfo.Scale.z)
		);

		auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
		auto& vertexindexInfo = potLoader.FindVertexIndicesInfo(info.first);
		MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), info.first, vertexindexInfo);
		MeshObject->ApplyRequestedLayers();
	}
}

void TestLevel::LoadGimmikObject()
{
	float doorRockRatio = 39.0625f;

	auto objmgr = ObjectManager::GetInstance();
	FBXMapLoader gimmickLoader;

	gimmickLoader.AddBasicObject(L"..\\..\\..\\Client\\Resources\\FBX\\Models\\GimmicksRAW.fbx");		// Mesh 로드
	gimmickLoader.ExtractMapInfo(L"..\\..\\..\\Client\\Resources\\FBX\\GimmickObjects.fbx");			// gimmik 로드

	auto& gimmickInfo = gimmickLoader.GetMapObjectInfo();

	for (auto& info : gimmickInfo)
	{
		const objectLocationInfo& locationInfo = info.second;
		MapObject* MeshObject{ nullptr };
		PillarObject* pillarObject{ nullptr };

		if (info.first == L"SM_Env_Rock_Square_Simple_01")		//돌담
		{
			auto boxObj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Gimmik_Rock", locationInfo.Position, Quat(0, 0, 0, 1),
				Vec3(locationInfo.Scale.x * doorRockRatio, locationInfo.Scale.y * doorRockRatio, locationInfo.Scale.z * doorRockRatio));

			auto boxBody = boxObj->GetComponent<RigidBody>(L"RigidBody");
			boxBody->AddCollider<BoxCollider>(boxObj->GetTransform()->GetScale());
			boxObj->ApplyRequestedLayers();
			boxBody->SetKinematic(false);
			boxBody->SetRigidBodySleep(false);
			boxBody->SetMass(100.f);

			auto boxCollider = boxBody->GetCollider(0);
			boxCollider->SetRestitution(0.4f);
			boxCollider->SetRestitutionCombineMode(PhysicsCombineMode::Min);
			boxCollider->SetFriction(0.5f);
			boxCollider->SetFrictionCombineMode(PhysicsCombineMode::Average);

			boxObj->ServerMessage_Init(true, false);
		}
		else if (info.first == L"SM_Env_Wall_Pillar_Large_Round_01")
		{
			std::wstring meshname = info.first;
			pillarObject = objmgr->AddGameObjectToLayer<PillarObject>(L"Layer_Gimmik_Pillar",
			   Vec3(locationInfo.Position.x * PX_SCALE_FACTOR, locationInfo.Position.y * PX_SCALE_FACTOR, locationInfo.Position.z * PX_SCALE_FACTOR),
			   Quat::FromEuler(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z),
			   Vec3(locationInfo.Scale.x, locationInfo.Scale.y, locationInfo.Scale.z)
			);

			auto pillarBody = pillarObject->GetComponent<RigidBody>(L"RigidBody");
			auto& vertexindexInfo = gimmickLoader.FindVertexIndicesInfo(info.first);
			pillarBody->AddCollider<MeshCollider>(pillarObject->GetTransform()->GetScale(), info.first, vertexindexInfo, true);
			pillarObject->Init_After_ColliderAttached();
		}
	}
}

void TestLevel::LoadTriggerObject()
{
	auto objMgr{ ObjectManager::GetInstance() };

	auto Portal1Obj{ objMgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(0.f, -550.f, 7510.f), Quat(0, 0, 0, 1), Vec3(500.f, 500.f, 500.f), false) };
	auto Portal1Body{ Portal1Obj->GetComponent<RigidBody>(L"RigidBody") };
	Portal1Body->AddCollider<BoxCollider>(Portal1Obj->GetTransform()->GetScale());
	Portal1Obj->SetTriggerAttribute(TriggerObject2::TRIGGERATTRIBUTE::PORTAL1);

	auto Portal2Obj{ objMgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(0.f, -550.f, 13020.f), Quat(0, 0, 0, 1), Vec3(500.f, 500.f, 500.f), false) };
	auto Portal2Body{ Portal2Obj->GetComponent<RigidBody>(L"RigidBody") };
	Portal2Body->AddCollider<BoxCollider>(Portal2Obj->GetTransform()->GetScale());
	Portal2Obj->SetTriggerAttribute(TriggerObject2::TRIGGERATTRIBUTE::PORTAL2);

	auto Portal3Obj{ objMgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(0.f, -1590.f, 18920.f), Quat(0, 0, 0, 1), Vec3(500.f, 500.f, 500.f), false) };
	auto Portal3Body{ Portal3Obj->GetComponent<RigidBody>(L"RigidBody") };
	Portal3Body->AddCollider<BoxCollider>(Portal3Obj->GetTransform()->GetScale());
	Portal3Obj->SetTriggerAttribute(TriggerObject2::TRIGGERATTRIBUTE::PORTAL3);

	auto Portal4Obj{ objMgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(15000.f, -1590.f, 24070.f), Quat(0, 0, 0, 1), Vec3(500.f, 500.f, 500.f), false) };
	auto Portal4Body{ Portal4Obj->GetComponent<RigidBody>(L"RigidBody") };
	Portal4Body->AddCollider<BoxCollider>(Portal4Obj->GetTransform()->GetScale());
	Portal4Obj->SetTriggerAttribute(TriggerObject2::TRIGGERATTRIBUTE::PORTAL4);

	auto Portal5Obj{ objMgr->AddGameObjectToLayer<TriggerObject2>(L"Layer_Map2", Vec3(16216.f, -3690.f, 40493.f), Quat(0, 0, 0, 1), Vec3(1000.f, 500.f, 500.f), false) };
	auto Portal5Body{ Portal5Obj->GetComponent<RigidBody>(L"RigidBody") };
	Portal5Body->AddCollider<BoxCollider>(Portal5Obj->GetTransform()->GetScale());
	Portal5Obj->SetTriggerAttribute(TriggerObject2::TRIGGERATTRIBUTE::PORTAL5);
}

void TestLevel::LoadDebugMap_Bridge()
{
	auto objmgr = ObjectManager::GetInstance();
	FBXMapLoader mapLoader;

	mapLoader.AddBasicObject(L"..\\..\\..\\Client\\Resources\\FBX\\Models\\BridgeObj.fbx");			// Mesh 로드
	mapLoader.ExtractMapInfo(L"..\\..\\..\\Client\\Resources\\FBX\\Bridge.fbx");					// Map 로드

	auto& mapInfo = mapLoader.GetMapObjectInfo();
	for (auto& info : mapInfo)
	{
		const objectLocationInfo& locationInfo = info.second;

		auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map",
		   Vec3(locationInfo.Position.x * PX_SCALE_FACTOR, locationInfo.Position.y * PX_SCALE_FACTOR, locationInfo.Position.z * PX_SCALE_FACTOR),
		   Quat::FromEuler(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z),
		   Vec3(locationInfo.Scale.x, locationInfo.Scale.y, locationInfo.Scale.z)
		);

		auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
		auto& vertexindexInfo = mapLoader.FindVertexIndicesInfo(info.first);
		MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), info.first, vertexindexInfo);
		MeshObject->ApplyRequestedLayers();
	}
}

void TestLevel::LoadDebugMap_Boulder()
{
	auto objmgr = ObjectManager::GetInstance();
	FBXMapLoader mapLoader;

	mapLoader.AddBasicObject(L"..\\..\\..\\Client\\Resources\\FBX\\Models\\RockRollingObj.fbx");			// Mesh 로드
	mapLoader.ExtractMapInfo(L"..\\..\\..\\Client\\Resources\\FBX\\RockRolling.fbx");			// Map 로드

	auto& mapInfo = mapLoader.GetMapObjectInfo();
	for (auto& info : mapInfo)
	{
		const objectLocationInfo& locationInfo = info.second;

		auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map",
		   Vec3(locationInfo.Position.x * PX_SCALE_FACTOR, locationInfo.Position.y * PX_SCALE_FACTOR, locationInfo.Position.z * PX_SCALE_FACTOR),
		   Quat::FromEuler(locationInfo.Rotation.x, locationInfo.Rotation.y, locationInfo.Rotation.z),
		   Vec3(locationInfo.Scale.x, locationInfo.Scale.y, locationInfo.Scale.z)
		);

		auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
		auto& vertexindexInfo = mapLoader.FindVertexIndicesInfo(info.first);
		MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), info.first, vertexindexInfo);
		MeshObject->ApplyRequestedLayers();
	}
}

void TestLevel::ThrowGimmik2Ball()
{
	auto objmgr = ObjectManager::GetInstance();

	float boulderPower = 100000.f;
	auto boulderObj = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Gimmik_Boulder", Vec3(16276.514, -140, 26758.246), Quat(0, 0, 0, 1), Vec3(300,300,300));
	auto boulderBody = boulderObj->GetComponent<RigidBody>(L"RigidBody");

	boulderBody->AddCollider<SphereCollider>(boulderObj->GetTransform()->GetScale());
	boulderObj->ApplyRequestedLayers();
	boulderBody->SetKinematic(false);
	boulderBody->SetRigidBodySleep(false);
	boulderBody->SetMass(500.f);

	auto boulderCollider = boulderBody->GetCollider(0);
	boulderCollider->SetRestitution(0.8f);
	boulderCollider->SetRestitutionCombineMode(PhysicsCombineMode::Max);
	boulderCollider->SetFriction(0.4f);
	boulderCollider->SetFrictionCombineMode(PhysicsCombineMode::Average);

	physx::PxVec3 dir = physx::PxVec3(16215, -2558, 35219) - boulderBody->GetPosition();
	dir.normalize();

	boulderBody->AddForce(ForceMode::Impulse, dir * boulderPower);

	boulderObj->ServerMessage_Init(false, true);      //0723
}

bool TestLevel::ThrowGimmik2Ball_RangeCheck()
{
	auto playerLists = ObjectManager::GetInstance()->GetLayer(L"Layer_Player")->GetGameObjects();
	for (auto& obj : playerLists)
	{
		Player* player = dynamic_cast<Player*>(obj);
		if (!player)
			continue;

		Vec3 pos = player->GetControllerPosition();
		if(TO_PX3(pos - Vec3(16028, -1140, 29001)).magnitude() < 1500)
			return true;
	}

	return false;
}

void TestLevel::TestFunction()
{
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

	using namespace std::chrono;
	static steady_clock::time_point bossSkillEndTime{ steady_clock::now() };
	static bool prevOnGround{ true };

	auto objMgr = ObjectManager::GetInstance();
	auto monsters = objMgr->GetLayer(L"Layer_Monster")->GetGameObjects();

	for (auto& monster : monsters)
	{
		auto mob{ dynamic_cast<Dragon*>(monster) };

		if (prevOnGround != mob->IsOnGround())
		{
			mob->DragonPattern_GroundHit();
			mob->SetState(Dragon::JUMP);

			//bossSkillEndTime = steady_clock::now();
		}

		prevOnGround = mob->IsOnGround();
	}

	//if (steady_clock::now() - bossSkillEndTime > 5s)
	//{
	//	for (auto& monster : monsters)
	//	{
	//		auto mob{ dynamic_cast<Monster*>(monster) };
	//
	//		if (mob->IsDead() == false)
	//		{
	//			mob->GetController()->SetSpaceKeyDown(true);
	//
	//			game::Message msg{ -1, ProtocolID::WR_JUMP_START_ACK };
	//			msg.objID = mob->GetID();
	//			game::MessageHandler::GetInstance()->PushSendMessage(msg);
	//		}
	//	}
	//}
}