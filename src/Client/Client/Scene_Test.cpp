﻿#include "pch.h"
#include <NetworkManager.h>
#include "Scene_Test.h"
#include "GameInstance.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "Animator.h"

#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "MeshData.h"

#include "Scripts.hpp"

#include <Network.h>

#include "FBXMapLoader.h"

std::shared_ptr<CScene> Scene_Test::TestScene(server::FBX_TYPE playerType)
{
	scene = make_shared<CScene>();

	CreateLayer();
	CreateComputeShader();
	CreateMainCamera();
	CreateUICamera();
	CreateSkyBox();
	CreateUI();
	CreateLights();
	CreateMap();
	CreateMapObjects();

	CreatePlayer(playerType);

	return scene;
}

void Scene_Test::Update()
{
	SendKeyInput();
}

void Scene_Test::LateUpdate()
{
	auto size{ scene->GetServerRequestQueueSize() };
	auto queue{ scene->GetServerRequest() };

	for (int32_t i = 0; i < size; ++i)
	{
		if (queue.empty() == true)
			break;

		auto request{ queue.front() };

		switch (request.ReadProtocol())
		{
			case ProtocolID::WR_ADD_ANIMATE_OBJ_ACK:
			{
				CreateAnimatedRemoteObject(request);
			}
			break;
			case ProtocolID::MY_ADD_OBJ_ACK:
			{
				ChangeNetworkObjectID(request);
			}
			break;
			case ProtocolID::WR_ADD_OBJ_ACK:
			{
				CreateRemoteObject(request);
			}
			break;
			case ProtocolID::WR_REMOVE_ACK:
			{
				RemoveObject(request);
			}
			break;
			default:
			break;
		}

		queue.pop_front();
	}

	scene->PopRequestQueue(size);
}

void Scene_Test::CreateLayer(void)
{
#pragma region LayerMask
	GGameInstance->SetLayerName(0, L"Default");
	GGameInstance->SetLayerName(1, L"UI");
#pragma endregion
}

void Scene_Test::CreateComputeShader(void)
{
#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion
}

void Scene_Test::CreateMainCamera(void)
{
#pragma region Camera
	{
		shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
		camera->AddComponent(make_shared<Camera_Basic>());
		camera->GetCamera()->SetFar(30000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, -500.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion
}

void Scene_Test::CreateUICamera(void)
{
#pragma region UI_Camera
	{
		shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 10000.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion
}

void Scene_Test::CreateSkyBox(void)
{
#pragma region SkyBox
	{
		shared_ptr<CGameObject> skybox = std::make_shared<CGameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky02", L"..\\Resources\\Texture\\Sky02.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion
}

void Scene_Test::CreateUI(void)
{
#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion
}

void Scene_Test::CreateLights(void)
{
#pragma region Directional Light
	{
		LightDesc lightDesc;
		lightDesc.vDirection = Vec3(0.f, -1.f, 0.5f);
		lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
		lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
		lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

		scene->AddDirectionalLight(lightDesc);
	}
#pragma endregion
}

void Scene_Test::CreateMap(void)
{
	FBXMapLoader mapLoader;
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Bones.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Floors.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Misc.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Optimized.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Pillars.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Rocks.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Walls.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Wood.fbx");

	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Items.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Props.fbx");
	//mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Weapons.fbx");

	//mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\Stage1.FBX");

	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Models\\Models.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Models\\Models3.fbx");
	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\NewStage.fbx");

	vector<shared_ptr<CGameObject>> mapObjects = mapLoader.GetMapObjectInfo();

	for (auto& mapObject : mapObjects)
	{
		mapObject->SetCheckFrustum(false);
		scene->AddGameObject(mapObject);
		scene->AddGameObject(mapObject);
	}
}

void Scene_Test::CreateMapObjects(void)
{
	//ObjectDesc objectDesc;
	//objectDesc.strName = L"moon";
	//objectDesc.strPath = L"..\\Resources\\FBX\\Moon\\moon.fbx";
	////objectDesc.strPath = L"..\\Resources\\FBX\\Dragon\\Dragon.fbx";
	//objectDesc.vPostion = Vec3(0.f, 0.f, 300.f);
	//objectDesc.vScale = Vec3(30.f, 30.f, 30.f);
	//objectDesc.script = nullptr;// std::make_shared<Monster_Dragon>();

	//std::shared_ptr<MeshData> meshData{ GET_SINGLE(Resources)->LoadFBX(objectDesc.strPath) };
	//std::vector<std::shared_ptr<CGameObject>> gameObjects{ meshData->Instantiate() };
	//std::shared_ptr<network::CNetwork> networkComponent{ std::make_shared<network::CNetwork>() };

	//for (auto& gameObject : gameObjects)
	//{
	//	gameObject->SetName(objectDesc.strName);
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(objectDesc.vPostion);
	//	gameObject->GetTransform()->SetLocalScale(objectDesc.vScale);
	//	//gameObject->AddComponent(objectDesc.script);
	//	gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 1);
	//	gameObject->AddComponent(networkComponent)15
	//}

	//scene->AddPlayer(gameObjects);
}

void Scene_Test::CreatePlayer(server::FBX_TYPE player)
{
	ObjectDesc objectDesc;
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);

	switch (player)
	{
		case server::FBX_TYPE::NANA:
		{
			objectDesc.strName = L"Nana";
		}
		break;
		case server::FBX_TYPE::MISTIC:
		{
			objectDesc.strName = L"Mistic";
		}
		break;
		case server::FBX_TYPE::CARMEL:
		{
			objectDesc.strName = L"Carmel";
		}
		break;
		default:
		break;
	}

	objectDesc.strPath = L"..\\Resources\\FBX\\Character\\" + objectDesc.strName + L"\\" + objectDesc.strName + L".fbx";
	objectDesc.script = std::make_shared<Player_Script>(player);

	std::vector<std::shared_ptr<CGameObject>> gameObjects = CreateAnimatedObject(objectDesc);
	gameObjects = AddNetworkToObject(gameObjects, server::OBJECT_TYPE::PLAYER);

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetObjectType(server::OBJECT_TYPE::PLAYER);
	}

	scene->AddPlayer(gameObjects);
}

void Scene_Test::CreateSphere()
{
	ObjectDesc objectDesc;
	objectDesc.strName = L"Mistic";
	objectDesc.strPath = L"..\\Resources\\FBX\\Sphere.fbx";
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);

	std::vector<std::shared_ptr<CGameObject>> gameObjects = CreateMapObject(objectDesc);
	gameObjects = AddNetworkToObject(gameObjects, server::OBJECT_TYPE::PLAYER_FIREBALL);

	Vec3 pos{ 1500.f, 100.f, -1000.f };
	for (auto& gameObject : gameObjects)
	{
		gameObject->GetTransform()->SetWorldVec3Position(pos);
		auto mat{ Matrix::CreateTranslation(pos) };
		gameObject->GetTransform()->SetWorldMatrix(mat);
	}

	scene->AddGameObject(gameObjects);
}

void Scene_Test::SendKeyInput()
{
	if (GET_NETWORK->IsSuccessfullyLoggedIn() == true)
	{
		GET_NETWORK->SendKeyInputPacket();
	}
}

void Scene_Test::ChangeNetworkObjectID(network::CPacket& packet)
{
	int32_t newID{ packet.ReadID() };
	int32_t oldID{ packet.Read<int32_t>() };

	m_objectIDMap[oldID] = newID;

	GET_NETWORK->ExchangeObjectID(oldID, newID);
}

void Scene_Test::CreateAnimatedRemoteObject(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	if (FindOverlappedObject(id) == true)
		return;

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Vec4 quat;
	quat.x = packet.Read<float>();
	quat.y = packet.Read<float>();
	quat.z = packet.Read<float>();
	quat.w = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	int32_t state{ packet.Read<int32_t>() };
	float updateTime{ packet.Read<float>() };

	server::OBJECT_TYPE objType{ packet.Read<server::OBJECT_TYPE>() };
	server::FBX_TYPE fbxType{ packet.Read<server::FBX_TYPE>() };

	ObjectDesc objectDesc;
	ClassifyObject(state, fbxType, objectDesc);

	if (objType == server::OBJECT_TYPE::PLAYER)
		objType = server::OBJECT_TYPE::REMOTE_PLAYER;

	std::vector<std::shared_ptr<CGameObject>> gameObjects{ CreateAnimatedObject(objectDesc) };
	gameObjects = AddNetworkToObject(gameObjects, objType, id);

	for (auto& gameObject : gameObjects)
	{
		Matrix matWorld{ gameObject->GetTransform()->GetWorldMatrix() };

		if (objType == server::OBJECT_TYPE::REMOTE_PLAYER)
			matWorld *= Matrix::CreateRotationY(XMConvertToRadians(180.f));

		matWorld *= Matrix::CreateTranslation(pos);
		gameObject->GetTransform()->SetWorldMatrix(matWorld);
		gameObject->SetObjectType(objType);
		gameObject->GetAnimator()->PlayFrame(state, updateTime);
	}

	AddObjectToScene(objType, gameObjects);
	GET_NETWORK->AddNetworkObject(id, gameObjects);

	switch (objType)
	{
		case server::OBJECT_TYPE::MONSTER:
		case server::OBJECT_TYPE::BOSS:
		{
			objectDesc.script->Start();
		}
		break;
		default:
		break;
	}
}

void Scene_Test::CreateRemoteObject(network::CPacket& packet)
{
	int32_t objID{ packet.ReadID() };

	//std::cout << objID << std::endl;
	//if (FindOverlappedObject(objID) == true)
	if (m_overlappedObjects.contains(objID) == true)
		return;

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	SimpleMath::Quaternion quat;
	quat.x = packet.Read<float>();
	quat.y = packet.Read<float>();
	quat.z = packet.Read<float>();
	quat.w = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	server::OBJECT_TYPE objType{ packet.Read<server::OBJECT_TYPE>() };
	server::FBX_TYPE fbxType{ packet.Read<server::FBX_TYPE>() };

	std::cout << objID << ", " << magic_enum::enum_name(objType) << std::endl;

	ObjectDesc objectDesc;
	ClassifyObject(-1, fbxType, objectDesc);

	network::NetworkGameObject gameObjects{ CreateMapObject(objectDesc) };
	gameObjects = AddNetworkToObject(gameObjects, objType, objID);

	for (auto& gameObject : gameObjects)
	{
		Matrix matWorld{ Matrix::CreateScale(scale) };
		matWorld *= Matrix::CreateFromQuaternion(quat);
		matWorld *= Matrix::CreateTranslation(pos);
		gameObject->GetTransform()->SetWorldMatrix(matWorld);

		if (magic_enum::enum_integer(server::OBJECT_TYPE::PLAYER_FIREBALL) <= magic_enum::enum_integer(objType)
			and magic_enum::enum_integer(objType) <= magic_enum::enum_integer(server::OBJECT_TYPE::MONSTER_POISONBALL))
		{
			gameObject->AddComponent(objectDesc.script);
		}

		gameObject->SetObjectType(objType);
	}

	AddObjectToScene(objType, gameObjects);
	GET_NETWORK->AddNetworkObject(objID, gameObjects);

	m_overlappedObjects.insert(objID);
}

void Scene_Test::RemoveObject(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };
	auto type{ packet.Read<server::OBJECT_TYPE>() };

	switch (type)
	{
		case server::OBJECT_TYPE::BOSS:
		{
			auto boss{ scene->GetBoss() };
			network::NetworkGameObject removeObject;

			for (auto& object : boss)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObject.push_back(object);
			}

			scene->RemoveBoss(removeObject);

			std::cout << "REMOVE BOSS" << std::endl;
		}
		break;
		case server::OBJECT_TYPE::MONSTER:
		{
			auto monster{ scene->GetMonster() };
			network::NetworkGameObject removeObject;

			for (auto& object : monster)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObject.push_back(object);
			}

			scene->RemoveMonster(removeObject);

			std::cout << "REMOVE MONSTER" << std::endl;
		}
		break;
		case server::OBJECT_TYPE::PLAYER_FIREBALL:
		case server::OBJECT_TYPE::PLAYER_ICEBALL:
		case server::OBJECT_TYPE::PLAYER_THUNDERBALL:
		case server::OBJECT_TYPE::PLAYER_POISONBALL:
		case server::OBJECT_TYPE::WEEPER_CAST1_BALL:
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL:
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL_SCATTER:
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL_NUCLEAR:
		case server::OBJECT_TYPE::WEEPER_CAST3_BALL:
		case server::OBJECT_TYPE::WEEPER_CAST4_BALL:
		case server::OBJECT_TYPE::MONSTER_FIREBALL:
		case server::OBJECT_TYPE::MONSTER_ICEBALL:
		case server::OBJECT_TYPE::MONSTER_THUNDERBALL:
		case server::OBJECT_TYPE::MONSTER_POISONBALL:
		{
			auto objects{ scene->GetNetworkObject() };
			network::NetworkGameObject removeObjects;

			for (auto& object : objects)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObjects.push_back(object);
			}

			scene->RemoveNetworkObject(removeObjects);

			//std::cout << id << ", " << magic_enum::enum_name(type) << std::endl;
			m_overlappedObjects.erase(id);
		}
		break;
		default:
		break;
	}

	GET_NETWORK->RemoveNetworkObject(id);
}

bool Scene_Test::FindOverlappedObject(int32_t id)
{
	for (auto& obj : GET_SCENE->GetNetworkObject())
	{
		if (obj->GetNetwork()->GetID() == id)
			return true;
	}

	return false;
}

void Scene_Test::ClassifyObject(int32_t stateIndex, server::FBX_TYPE type, ObjectDesc& objectDesc)
{
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);

	switch (type)
	{
		case server::FBX_TYPE::NANA:
		{
			objectDesc.strName = L"Nana";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Nana\\Nana.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type);
			std::wcout << objectDesc.strName << std::endl;
		}
		break;
		case server::FBX_TYPE::MISTIC:
		{
			objectDesc.strName = L"Mistic";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Mistic\\Mistic.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type);
			std::wcout << objectDesc.strName << std::endl;
		}
		break;
		case server::FBX_TYPE::CARMEL:
		{
			objectDesc.strName = L"Carmel";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Carmel\\Carmel.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type);
			std::wcout << objectDesc.strName << std::endl;
		}
		break;
		case server::FBX_TYPE::WEEPER1:
		case server::FBX_TYPE::WEEPER2:
		case server::FBX_TYPE::WEEPER3:
		case server::FBX_TYPE::WEEPER4:
		case server::FBX_TYPE::WEEPER5:
		case server::FBX_TYPE::WEEPER6:
		case server::FBX_TYPE::WEEPER7:
		{
			objectDesc.strName = L"Weeper" + std::to_wstring(magic_enum::enum_integer(type) - magic_enum::enum_integer(server::FBX_TYPE::WEEPER1) + 1);
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Weeper\\" + objectDesc.strName + L".fbx";
			objectDesc.script = std::make_shared<Monster_Weeper>(stateIndex);
		}
		break;
		case server::FBX_TYPE::WEEPER_EMISSIVE:
		{
			objectDesc.strName = L"Weeper_Emissive";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Weeper\\Weeper Emissive.fbx";
			objectDesc.script = std::make_shared<Monster_Weeper>(stateIndex);
		}
		break;
		case server::FBX_TYPE::BLUE_GOLEM:
		{
			objectDesc.strName = L"Blue Golem";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\MoltenGolem\\Blue Golem.fbx";
			objectDesc.script = std::make_shared<Monster_Golem>(stateIndex);
		}
		break;
		case server::FBX_TYPE::GREEN_GOLEM:
		{
			objectDesc.strName = L"Green Golem";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\MoltenGolem\\Green Golem.fbx";
			objectDesc.script = std::make_shared<Monster_Golem>(stateIndex);
		}
		break;
		case server::FBX_TYPE::RED_GOLEM:
		{
			objectDesc.strName = L"Red Golem";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\MoltenGolem\\Blue Golem.fbx";
			objectDesc.script = std::make_shared<Monster_Golem>(stateIndex);
		}
		break;
		case server::FBX_TYPE::BLACK_SCORPION:
		{
			objectDesc.strName = L"Black Scorpion";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Black Scorpion.fbx";
			objectDesc.script = std::make_shared<Monster_Scorpion>(stateIndex);
		}
		break;
		case server::FBX_TYPE::ORANGE_SCORPION:
		{
			objectDesc.strName = L"Orange Scorpion";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Orange Scorpion.fbx";
			objectDesc.script = std::make_shared<Monster_Scorpion>(stateIndex);
		}
		break;
		case server::FBX_TYPE::PURPLE_SCORPION:
		{
			objectDesc.strName = L"Purple Scorpion";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Purple Scorpion.fbx";
			objectDesc.script = std::make_shared<Monster_Scorpion>(stateIndex);
		}
		break;
		case server::FBX_TYPE::RED_SCORPION:
		{
			objectDesc.strName = L"Red Scorpion";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Red Scorpion.fbx";
			objectDesc.script = std::make_shared<Monster_Scorpion>(stateIndex);
		}
		break;
		case server::FBX_TYPE::PLAYER_FIREBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>();
		}
		break;
		case server::FBX_TYPE::PLAYER_ICEBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>();
		}
		break;
		case server::FBX_TYPE::PLAYER_THUNDERBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>();
		}
		break;
		case server::FBX_TYPE::PLAYER_POISONBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>();
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST1_BALL:
		{
			objectDesc.strName = L"Weeper Cast1";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST2_BALL:
		case server::FBX_TYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			objectDesc.strName = L"Weeper Cast2";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Stone.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
			objectDesc.vScale = { 0.5f, 0.5f, 0.5f };
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST3_BALL:
		{
			objectDesc.strName = L"Weeper Cast3";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST4_BALL:
		{
			objectDesc.strName = L"Weeper Cast4";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Stone Bullet2.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
		}
		break;
		case server::FBX_TYPE::MONSTER_ICEBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
		}
		break;
		case server::FBX_TYPE::MONSTER_THUNDERBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Stone Spike.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
		}
		break;
		case server::FBX_TYPE::MONSTER_POISONBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
		}
		break;
		case server::FBX_TYPE::MONSTER_DARKBALL:
		{
			objectDesc.strName = L"Stone Bullet2";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
		}
		break;
		default:
		break;
	}
}

void Scene_Test::AddObjectToScene(server::OBJECT_TYPE type, std::vector<std::shared_ptr<CGameObject>>& gameObjects)
{
	switch (type)
	{
		case server::OBJECT_TYPE::REMOTE_PLAYER:
		{
			scene->AddPlayer(gameObjects);
			std::cout << "ADD REMOTE PLAYER" << std::endl;
		}
		break;
		case server::OBJECT_TYPE::MONSTER:
		{
			scene->AddMonster(gameObjects);
			std::cout << "ADD MONSTER" << std::endl;
		}
		break;
		case server::OBJECT_TYPE::BOSS:
		{
			scene->AddBoss(gameObjects);
			std::cout << "ADD BOSS" << std::endl;
		}
		break;
		default:
		{
			scene->AddNetworkObject(gameObjects);
		}
		break;
	}
}

std::vector<std::shared_ptr<CGameObject>> Scene_Test::CreateMapObject(ObjectDesc& objectDesc)
{
	shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(objectDesc.strPath);

	vector<shared_ptr<CGameObject>> gameObjects = meshData->Instantiate();

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetName(objectDesc.strName);
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(objectDesc.vPostion);
		gameObject->GetTransform()->SetLocalScale(objectDesc.vScale);
		gameObject->AddComponent(objectDesc.script);
		gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 1);
	}

	return gameObjects;
}

std::vector<std::shared_ptr<CGameObject>> Scene_Test::CreateAnimatedObject(ObjectDesc& objectDesc)
{
	vector<shared_ptr<CGameObject>> gameObjects = CreateMapObject(objectDesc);

	for (auto& gameObject : gameObjects)
	{
		gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
	}

	return gameObjects;
}

std::vector<std::shared_ptr<CGameObject>> Scene_Test::AddNetworkToObject(std::vector<std::shared_ptr<CGameObject>> objects, server::OBJECT_TYPE objectType, int32_t id)
{
	std::shared_ptr<network::CNetwork> networkComponent{ std::make_shared<network::CNetwork>(objectType, id) };

	for (auto& gameObject : objects)
	{
		gameObject->AddComponent(networkComponent);
	}

	return objects;
}
