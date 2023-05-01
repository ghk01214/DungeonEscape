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
	//Player* player{ objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5)) };
	//auto PlayerObject = objmgr->AddGameObjectToLayer<Player>(L"Layer_Player", 3, Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5));

#pragma region Plane
	/*auto MapPlaneObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 2, 0), Quat(0, 0, 0, 1), Vec3(100, 2, 100));
	auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
	MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());*/
#pragma endregion

#pragma region Sphere
	/*auto SphereObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(20, 20, 20), Quat(0, 0, 0, 1), Vec3(2,2,2));
	auto SphereBody = SphereObject->GetComponent<RigidBody>(L"RigidBody");
	SphereBody->SetKinematic(false);
	SphereBody->SetCCDFlag(true);
	SphereBody->SetRigidBodySleep(false);
	SphereBody->AddCollider<SphereCollider>(SphereObject->GetTransform()->GetScale());

	SphereBody->SetAngularDamping(0.00001f);
	SphereBody->SetLinearDamping(0.15f);
	SphereBody->SetMass(SphereBody->GetMass() * 0.20f);*/
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
	/*auto MeshObject = objmgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(-10, 2, 0), Quat(0, 0, 0, 1), Vec3(1, 1, 1));
	auto MeshBody = MeshObject->GetComponent<RigidBody>(L"RigidBody");
	MeshBody->AddCollider<MeshCollider>(MeshObject->GetTransform()->GetScale(), L"Moon", L"..\\Resources\\FBX\\Moon\\moon.fbx");*/
#pragma endregion

#pragma region MAP
	// 성욱
#pragma endregion
}

void TestLevel::Update(double timeDelta)
{
	MessageHandler::GetInstance()->ExecuteMessage();
}

void TestLevel::LateUpdate(double timeDelta)
{
}

void TestLevel::Release()
{
}
