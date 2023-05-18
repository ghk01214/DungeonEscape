#include "pch.h"
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

#include "TestCameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "MeshData.h"

#include "Monster_Script.h"
#include "Camera_Script.h"
#include "Player_Script.h"

#include <Network.h>

#include "FBXMapLoader.h"

std::shared_ptr<CScene> Scene_Test::TestScene(void)
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

	CreatePlayer();

	return scene;
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
				CreateRemotePlayer(request);

				std::cout << "ADD REMOTE[" << request.ReadID() << "]" << std::endl << std::endl;
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
			case ProtocolID::MY_ADD_OBJ_COLLIDER_ACK:
			{
				ChangeColliderID(request);
			}
			break;
			case ProtocolID::WR_ADD_OBJ_COLLIDER_ACK:
			{
				AddColliderToObject(request);
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
		camera->GetCamera()->SetFar(5000.f);
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
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
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
		lightDesc.vDirection = Vec3(0, -1, 1.f);
		lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
		lightDesc.vAmbient = Vec3(0.1f, 0.1f, 0.1f);
		lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

		scene->AddDirectionalLight(lightDesc);
	}
#pragma endregion
}

void Scene_Test::CreateMap(void)
{
	FBXMapLoader mapLoader;
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Bones.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Floors.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Misc.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Optimized.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Pillars.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Rocks.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Walls.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Environments\\Wood.fbx");

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\Stage2.FBX");

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

void Scene_Test::CreatePlayer(void)
{
	ObjectDesc objectDesc;
	objectDesc.strName = L"Mistic";
	objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Mistic\\Mistic.fbx";
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);
	objectDesc.script = std::make_shared<Player_Mistic>();

	std::vector<std::shared_ptr<CGameObject>> gameObjects = CreateAnimatedObject(objectDesc);
	gameObjects = AddNetworkToObject(gameObjects, server::OBJECT_TYPE::PLAYER);

	scene->AddPlayer(gameObjects);
}

void Scene_Test::CreateRemotePlayer(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

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

	int32_t aniIndex{ packet.Read<int32_t>() };
	float aniFrame{ packet.Read<float>() };

	server::FBX_TYPE type{ packet.Read<server::FBX_TYPE>() };

	std::wstring fbxName{};

	if (type == server::FBX_TYPE::MISTIC)
		fbxName = L"Mistic";

	std::cout << std::format("ID : {}\n", id);
	std::cout << std::format("pos : {}, {}, {}\n", pos.x, pos.y, pos.z);
	std::cout << std::format("quat : {}, {}, {}, {}\n", quat.x, quat.y, quat.z, quat.w);
	std::cout << std::format("scale : {}, {}, {}\n\n", scale.x, scale.y, scale.z);

	ObjectDesc objectDesc;
	objectDesc.strName = fbxName;
	objectDesc.strPath = L"..\\Resources\\FBX\\Character\\" + fbxName + L"\\" + fbxName + L".fbx";
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);
	objectDesc.script = std::make_shared<Player_Mistic>();

	std::vector<std::shared_ptr<CGameObject>> gameObjects = CreateAnimatedObject(objectDesc);
	gameObjects = AddNetworkToObject(gameObjects, server::OBJECT_TYPE::REMOTE_PLAYER, id);

	for (auto& gameObject : gameObjects)
	{
		gameObject->GetTransform()->SetWorldVec3Position(pos);
		auto mat{ Matrix::CreateTranslation(pos) };
		gameObject->GetTransform()->SetWorldMatrix(mat);
		gameObject->GetAnimator()->Play(aniIndex, aniFrame);
	}

	scene->AddPlayer(gameObjects);
	GET_NETWORK->AddNetworkObject(id, gameObjects);
}

void Scene_Test::ChangeNetworkObjectID(network::CPacket& packet)
{
	int32_t newID{ packet.ReadID() };
	int32_t oldID{ packet.Read<int32_t>() };

	m_objectIDMap[oldID] = newID;

	GET_NETWORK->ExchangeObjectID(oldID, newID);
}

void Scene_Test::CreateRemoteObject(network::CPacket& packet)
{
	int32_t objID{ packet.ReadID() };

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

	server::FBX_TYPE fbxType{ packet.Read<server::FBX_TYPE>() };
	server::OBJECT_TYPE objType{ packet.Read<server::OBJECT_TYPE>() };

	std::wstring fbxName{};

	if (fbxType == server::FBX_TYPE::MISTIC)
		fbxName = L"Mistic";

	ObjectDesc objectDesc;
	objectDesc.strName = fbxName;
	objectDesc.strPath = L"..\\Resources\\FBX\\Character\\" + fbxName + L"\\" + fbxName + L".fbx";
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);
	objectDesc.script = std::make_shared<Player_Mistic>();

	network::NetworkGameObject gameObjects{ CreateMapObject(objectDesc) };
	gameObjects = AddNetworkToObject(gameObjects, objType, objID);

	for (auto& gameObject : gameObjects)
	{
		gameObject->GetTransform()->SetWorldVec3Position(pos);
		auto mat{ Matrix::CreateTranslation(pos) };
		gameObject->GetTransform()->SetWorldMatrix(mat);
	}

	scene->AddGameObject(gameObjects);
	GET_NETWORK->AddNetworkObject(objID, gameObjects);
}

void Scene_Test::ChangeColliderID(network::CPacket& packet)
{
	int32_t tempObjID{ packet.Read<int32_t>() };
	int32_t newID{ packet.Read<int32_t>() };
	int32_t oldID{ packet.Read<int32_t>() };
	int32_t objID{ m_objectIDMap[tempObjID] };
	bool lastCollider{ packet.Read<bool>() };

	auto objects{ GET_NETWORK->GetNetworkObject(objID) };

	for (auto& object : objects)
	{
		object->ChangeColliderID(oldID, newID);
	}
}

void Scene_Test::AddColliderToObject(network::CPacket& packet)
{
	int32_t objID{ packet.ReadID() };

	Collider collider;

	collider.id = packet.Read<int32_t>();
	collider.type = packet.Read<server::COLLIDER_TYPE>();

	collider.pos.x = packet.Read<float>();
	collider.pos.y = packet.Read<float>();
	collider.pos.z = packet.Read<float>();

	collider.quat.x = packet.Read<float>();
	collider.quat.y = packet.Read<float>();
	collider.quat.z = packet.Read<float>();
	collider.quat.w = packet.Read<float>();

	collider.scale.x = packet.Read<float>();
	collider.scale.y = packet.Read<float>();
	collider.scale.z = packet.Read<float>();

	auto objects{ GET_NETWORK->GetNetworkObject(objID) };

	// 개량 필요
	for (auto& object : objects)
	{
		object->SetCollider(collider.id, collider);
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
		gameObject->GetAnimator()->Play(Player_Mistic::IDLE_A);
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
