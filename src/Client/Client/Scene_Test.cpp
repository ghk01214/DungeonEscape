﻿#include "pch.h"
#include "Scene_Test.h"

#include <NetworkManager.h>
#include <SceneManager.h>
#include <GameObject.h>
#include <Engine.h>
#include <Resources.h>
#include <Material.h>
#include <MeshData.h>
#include <MeshRenderer.h>
#include <Transform.h>
#include <Camera.h>
#include <Light.h>
#include <Animator.h>
#include <BillBoard.h>
#include <ParticleSystem.h>
#include <Terrain.h>
#include <SphereCollider.h>
#include <Effect.h>
#include <Network.h>
#include <FBXMapLoader.h>
#include <Input.h>
#include <EffectManager.h>
#include <SoundManager.h>
#include <Timer.h>

#include "Scripts.hpp"
#include "Skill_Bomb_Script.h"
#include "VolumeSlider_Script.h"

#include "EffectManager.h"
#include "SoundManager.h"
#include "Timer.h"

#include "InfoUI_Script.h"
#include "Fade_Script.h"

#include "FontManager.h"
#include "Font.h"

#include "PortalUI_Script.h"
#include "Cinematic_Script.h"
#include "Movement_Script.h"
#include "Magic_Artifact_Script.h"
#include "OneTimeDialogue_Script.h"

Scene_Test::Scene_Test()
{

}

void Scene_Test::Awake()
{
	__super::Awake();
}

void Scene_Test::Start()
{
	__super::Start();

	if (playMusic == true)
	{
		GET_SINGLE(CSoundMgr)->StopAll();
		GET_SINGLE(CSoundMgr)->PlayBGM(L"World.ogg");
	}
}

void Scene_Test::Update()
{
	__super::Update();

	SendKeyInput();
	CheckMapMove();
}

void Scene_Test::LateUpdate()
{
	__super::LateUpdate();

	auto size{ GetServerRequestQueueSize() };
	auto queue{ GetServerRequest() };

	for (int32_t i = 0; i < size; ++i)
	{
		if (queue.empty() == true)
			break;

		auto packet{ queue.front() };

		switch (packet.ReadProtocol())
		{
			case ProtocolID::WR_ADD_ANIMATE_OBJ_ACK:
			{
				CreateAnimatedRemoteObject(packet);
			}
			break;
			case ProtocolID::MY_ADD_OBJ_ACK:
			{
				ChangeNetworkObjectID(packet);
			}
			break;
			case ProtocolID::WR_ADD_OBJ_ACK:
			{
				CreateRemoteObject(packet);
			}
			break;
			case ProtocolID::WR_REMOVE_ACK:
			{
				RemoveObject(packet);
			}
			break;
			case ProtocolID::WR_SKILL_HIT_ACK:
			{

			}
			break;
			case ProtocolID::WR_RENDER_EFFECT_ACK:
			{
				PlayEffect(packet);
			}
			break;
			case ProtocolID::WR_CHANGE_SOUND_ACK:
			{
				ChangeSound(packet);
			}
			break;
			case ProtocolID::WR_TRIGGER_INTERACTION_ACK:
			{
				TriggerBehaviour(packet);
			}
			break;
			case ProtocolID::WR_TRIGGER_INTERACTION_COUNT_ACK:
			{
				TriggerInteractionCount(packet);
			}
			break;
			case ProtocolID::WR_PLAY_CUT_SCENE_ACK:
			{
				PlayCutScene(packet);
			}
			break;
			default:
			break;
		}

		queue.pop_front();
	}

	PopRequestQueue(size);
}

#pragma region
void Scene_Test::FinalUpdate()
{
	__super::FinalUpdate();
}

void Scene_Test::Render()
{
	__super::Render();
}

void Scene_Test::CreateLayer()
{
	GGameInstance->SetLayerName(0, L"Default");
	GGameInstance->SetLayerName(1, L"UI");
	//GGameInstance->SetLayerName(2, L"Senematic");

	GET_SINGLE(EffectManager)->SetSceneIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"Default"));
	GET_SINGLE(FontManager)->SetUIIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	GET_SINGLE(FontManager)->SetObjectIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"Default"));
	GET_SINGLE(SceneManager)->SetUIIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
}

void Scene_Test::CreateComputeShader(void)
{
#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV ??Texture ??밴쉐
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// ?怨뺤쟿??域밸챶竊?(1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion
}

void Scene_Test::CreateMainCamera(shared_ptr<CScene> pScene)
{
	shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->SetName(L"Main_Camera");
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45??


//#define MOVEMENT

#ifdef MOVEMENT
	shared_ptr<Movement_Script> pMovementScript = make_shared<Movement_Script>();
	camera->AddComponent(pMovementScript);
#else
	shared_ptr<Camera_Basic> pCameraScript = make_shared<Camera_Basic>();
	m_InfoUIScript->SetCameraScript(pCameraScript);
	camera->AddComponent(pCameraScript);

	shared_ptr<Cinematic_Script> pSenematicScript = make_shared<Cinematic_Script>();
	m_scenematicScript = pSenematicScript;
	pSenematicScript->SetScript(pCameraScript);
	camera->AddComponent(pSenematicScript);
#endif




	camera->GetCamera()->SetFar(30000.f);
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, -500.f));
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI????筌〓씮??
	pScene->AddGameObject(camera);
}

void Scene_Test::CreateUICamera(shared_ptr<CScene> pScene)
{
	shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->SetName(L"Orthographic_Camera");
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
	camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	camera->GetCamera()->SetCullingMaskAll(); // ???袁㏉?
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI筌?筌〓씮??

	camera->AddComponent(m_InfoUIScript);

	pScene->AddGameObject(camera);
}

void Scene_Test::CreateSkyBox(shared_ptr<CScene> pScene)
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
		pScene->AddGameObject(skybox);
	}
#pragma endregion
}

void Scene_Test::CreateLights(shared_ptr<CScene> pScene)
{
	LightDesc lightDesc;
	lightDesc.vDirection = Vec3(0.f, -1.f, 0.f);
	lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
	lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
	lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

	pScene->AddDirectionalLight(lightDesc);
}

void Scene_Test::CreatePlayer(shared_ptr<CScene> pScene, server::FBX_TYPE player)
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

	pScene->AddPlayer(gameObjects);
	GET_NETWORK->AddNetworkComponent(gameObjects);

	Matrix matWorld = Matrix::CreateTranslation(Vec3(0.f, -1500.f, 0.f));
	for (auto& object : gameObjects)
	{
		object->GetTransform()->SetWorldMatrix(matWorld);
	}

}

void Scene_Test::CreateSphere(shared_ptr<CScene> pScene)
{
	ObjectDesc objectDesc;
	objectDesc.strName = L"Mistic";
	objectDesc.strPath = L"..\\Resources\\FBX\\Temp\\gateway.fbx";
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);

	std::vector<std::shared_ptr<CGameObject>> gameObjects = CreateMapObject(objectDesc);
	//gameObjects = AddNetworkToObject(gameObjects, server::OBJECT_TYPE::PLAYER_FIREBALL);

	Vec3 pos{ 0.f, 0.f, 0.f };

	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Alpha_Blend_Object");

	for (auto& gameObject : gameObjects)
	{
		for (uint32 i = 0; i < gameObject->GetMeshRenderer()->GetMaterialSize(); ++i)
		{
			gameObject->GetMeshRenderer()->GetMaterial(i)->SetShader(shader);
		}
		gameObject->GetTransform()->SetWorldVec3Position(pos);
		auto mat{ Matrix::CreateTranslation(pos) };
		gameObject->GetTransform()->SetWorldMatrix(mat);
	}

	pScene->AddGameObject(gameObjects);
}

void Scene_Test::SendKeyInput()
{
	if (GET_NETWORK->IsSuccessfullyLoggedIn() == true)
		GET_NETWORK->SendKeyInputPacket();
}
#pragma endregion

void Scene_Test::CreateUI(shared_ptr<CScene> pScene)
{
	//CreateHPnSPBar();
	CreateOneTimeDialogue();
}

void Scene_Test::CreateMRTUI(shared_ptr<CScene> pScene)
{
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<CGameObject> obj = make_shared<CGameObject>();
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
		pScene->AddGameObject(obj);
	}
}

void Scene_Test::CreatePortalUI(shared_ptr<CScene> pScene)
{
	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
	m_portalUIScript = std::make_shared<PortalUI_Script>();
	obj->AddComponent(m_portalUIScript);

	pScene->AddGameObject(obj);
}

void Scene_Test::CreateHPnSPBar()
{
	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	// 獄쏄퀗瑗?獄?
	for (int32_t i = 0; i < 2; ++i)
	{
		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		obj->AddComponent(std::make_shared<Transform>());

		float yPos{ -(height / 2.f) + 10.f * (i * 2 + 1) };

		obj->GetTransform()->SetLocalScale(Vec3(250.f, 20.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, yPos, 1.1f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}

		{
			std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
			std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"Bar");

			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetFloat(2, 1.f);
			meshRenderer->SetMaterial(material);
		}

		obj->AddComponent(meshRenderer);

		std::shared_ptr<Bar_Script> behaviour = std::make_shared<Bar_Script>();
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"Bar"));

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}

	// HP & SP
	for (int32_t i = 0; i < 2; ++i)
	{
		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		obj->AddComponent(std::make_shared<Transform>());

		float yPos{ 0.f };
		std::wstring name{};

		if (i == 0)
		{
			yPos = -(height / 2.f) + 31.f;
			name = L"HP";
		}
		else
		{
			yPos = -(height / 2.f) + 11.f;
			name = L"MP";
		}

		obj->GetTransform()->SetLocalScale(Vec3(240.f, 9.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, yPos, 1.f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}

		{
			std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
			std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(name);

			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetFloat(2, 1.f);
			meshRenderer->SetMaterial(material);
		}

		obj->AddComponent(meshRenderer);

		std::shared_ptr<VolumeSlider_Script> behaviour = std::make_shared<VolumeSlider_Script>();
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name));

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}
}

void Scene_Test::CreateOneTimeDialogue()
{
	{
		std::shared_ptr<CGameObject> obj{ std::make_shared<CGameObject>() };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		obj->AddComponent(std::make_shared<Transform>());

		auto pos{ GetRatio(0.f, 50.f) };

		obj->GetTransform()->SetLocalScale(Vec3(916.f, 106.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 1.1f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh{ GET_SINGLE(Resources)->LoadRectangleMesh() };
			meshRenderer->SetMesh(mesh);
		}

		{
			std::shared_ptr<Shader> shader{ GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture") };
			std::shared_ptr<Texture> texture{ GET_SINGLE(Resources)->Get<Texture>(L"Pillar Hint") };

			std::shared_ptr<Material> material{ std::make_shared<Material>() };
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetFloat(2, 0.f);
			meshRenderer->SetMaterial(material);
		}

		obj->AddComponent(meshRenderer);

		std::shared_ptr<OneTimeDialogue_Script> behaviour{ std::make_shared<OneTimeDialogue_Script>("PILLAR_HINT") };
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"Bar"));
		m_oneTimeDialogueScript["PILLAR_HINT"] = behaviour;

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}
}

void Scene_Test::CreateFade(shared_ptr<CScene> pScene)
{
	shared_ptr<CGameObject> gameObject = make_shared<CGameObject>();
	gameObject->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

	gameObject->AddComponent(make_shared<Transform>());

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	gameObject->GetTransform()->SetLocalScale(Vec3(width, height, 1.f));
	gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 1.f));

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();

	meshRenderer->SetMesh(mesh);
	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");

	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"Lobby_InGame");
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(shader);
	material->SetTexture(0, texture);
	meshRenderer->SetMaterial(material);

	gameObject->AddComponent(meshRenderer);


	shared_ptr<Fade_Script> fade_script = make_shared<Fade_Script>();
	vector<std::shared_ptr<Texture>> vTextures;
	vTextures.push_back(texture);

	fade_script->SetLogoInfo(1.f, 1.f, vTextures);
	gameObject->AddComponent(fade_script);

	m_fadeScript = fade_script;

	pScene->AddGameObject(gameObject);
}

void Scene_Test::CreateMap(shared_ptr<CScene> pScene)
{
	FBXMapLoader mapLoader;

	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Models\\Models.fbx");
	mapLoader.AddBasicObject(L"..\\Resources\\FBX\\Models\\Models2.fbx");

	//mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\Client.fbx");

	//for (auto& mapObject : mapLoader.GetMapObjectInfo())
	//{
	//	mapObject->SetCheckFrustum(false);
	//	mapObject->SetStatic(false);
	//	pScene->AddMapObject(mapObject);
	//}

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\SplitMap\\Client\\StartRoom.fbx");
	PushMapData(MAP_TYPE::StartRoom, mapLoader.GetMapObjectInfo());

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\SplitMap\\Client\\FirstBoss.fbx");
	PushMapData(MAP_TYPE::FirstBoss, mapLoader.GetMapObjectInfo());

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\SplitMap\\Client\\Cave.fbx");
	PushMapData(MAP_TYPE::Cave, mapLoader.GetMapObjectInfo());

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\SplitMap\\Client\\SecondRoom_Bridge_SecondBoss.fbx");
	PushMapData(MAP_TYPE::SecondRoom_Bridge_SecondBoss, mapLoader.GetMapObjectInfo());

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\SplitMap\\Client\\ThirdRoom_RockRolling.fbx");
	PushMapData(MAP_TYPE::ThirdRoom_RockRolling, mapLoader.GetMapObjectInfo());

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\SplitMap\\Client\\LastBoss_TreasureRoom.fbx");
	PushMapData(MAP_TYPE::LastBoss_TreasureRoom, mapLoader.GetMapObjectInfo());

	//m_eNextMapType = MAP_TYPE::StartRoom;
	//MoveMap(MAP_TYPE::StartRoom);
	m_eNextMapType = MAP_TYPE::SecondRoom_Bridge_SecondBoss;
	MoveMap(MAP_TYPE::SecondRoom_Bridge_SecondBoss);
}

void Scene_Test::CreateSkill(shared_ptr<CScene> pScene)
{
	// ??삵닏??븍뱜 ??밴쉐
	vector<shared_ptr<CGameObject>> gameObjects = CreateSkillBase(L"BombBase", L"..\\Resources\\FBX\\Skill\\Sphere\\Sphere_Yellow.fbx");

	for (auto& object : gameObjects)
	{
		object->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		object->AddComponent(std::make_shared<Bomb_Script>());
	}

	// ??삵닏??븍뱜 ?곕떽?
	pScene->AddGameObject(gameObjects);
}

void Scene_Test::CreateBillBoard(shared_ptr<CScene> pScene)
{
	// ??용뮞????밴쉐
	vector<shared_ptr<Texture>> textures = GET_SINGLE(Resources)->LoadTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Sprite\\Fire.png", 64);

	// ??삵닏??븍뱜 ??밴쉐
	shared_ptr<CGameObject> gameObjects = CreateBillBoardBase(textures, 0.0001f);

	gameObjects->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));

	// ??삵닏??븍뱜 ?곕떽?
	pScene->AddGameObject(gameObjects);
}

void Scene_Test::CreateEffect(shared_ptr<CScene> pScene)
{
	// ??용뮞????밴쉐
	vector<shared_ptr<Texture>> textures = GET_SINGLE(Resources)->GetEffectTextures(L"Effect_CircleFrame_DarkBlue");

	// ??삵닏??븍뱜 ??밴쉐
	shared_ptr<CGameObject> gameObjects = CreateEffectBase(textures, 0.0001f);

	gameObjects->GetTransform()->SetLocalScale(Vec3(200.f, 200.f, 1.f));
	Matrix matWorld = Matrix::CreateTranslation(0.f, 0.f, 0.f);
	gameObjects->GetTransform()->SetWorldMatrix(matWorld);

	// ??삵닏??븍뱜 ?곕떽?
	pScene->AddGameObject(gameObjects);
}

void Scene_Test::CreateMagicArtifactEffect(shared_ptr<CScene> pScene)
{
	vector<shared_ptr<Texture>> textures = GET_SINGLE(Resources)->GetEffectTextures(L"Effect_Artifact_Protection");

	float magicCount{ 4 * 6 };

	for (int i = 0; i < magicCount; ++i)
	{
		float height{ 300.f * (i / 4) };
		shared_ptr<CGameObject> gameObject = CreateArtifactBase(textures);
		std::shared_ptr<Magic_Artifact_Script> behaviour = std::make_shared<Magic_Artifact_Script>();

		behaviour->SetStartRotation(360.f / 4 * i);	// 최초 회전 각도
		behaviour->SetRotationSpeed(30.f);	// 초당 몇도 회전하는지

		behaviour->SetTexture(textures);	// 텍스쳐 정보
		behaviour->SetSize(Vec2(300.f, 300.f));	// 텍스쳐의 크기

		behaviour->SetDistanceFromPoint(180.f);	// 중점으로부터 거리
		behaviour->SetTargetPoint(Vec3(6980.f, -1640.f + height, 21180.f));	// 중점 위치

		behaviour->SetPassingTime(0.05f);	// 텍스쳐 1장을 넘어가는데 걸리는 시간

		gameObject->AddComponent(behaviour);

		pScene->AddGameObject(gameObject);

		m_artifactMagicScript.push_back(behaviour);
	}
}

std::shared_ptr<CGameObject> Scene_Test::CreateBillBoardBase(vector<shared_ptr<Texture>> textures, float fPassingTime)
{
	// ??삵닏??븍뱜 ??밴쉐
	shared_ptr<CGameObject> gameObjects = std::make_shared<CGameObject>();

	// ?袁⑺뒄 ??쇱젟
	gameObjects->AddComponent(make_shared<Transform>());

	// ??슢???筌ｌ꼶??
	gameObjects->AddComponent(make_shared<BillBoard>());

	// ??슢?????용뮞????쇱젟
	gameObjects->GetBillBoard()->SetBBInfo(BB_TYPE::ATLAS, textures, fPassingTime);

	// MeshRenderer - ??而??筌롫뗄?? ??용뮞????쇱젟
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();

	meshRenderer->SetMesh(mesh);
	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"BillBoard_Texture");

	shared_ptr<Texture> texture = gameObjects->GetBillBoard()->GetTexture();
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(shader);
	material->SetTexture(0, texture);
	meshRenderer->SetMaterial(material);

	gameObjects->AddComponent(meshRenderer);

	return gameObjects;
}

std::shared_ptr<CGameObject> Scene_Test::CreateEffectBase(vector<shared_ptr<class Texture>> textures, float fPassingTime)
{
	// ??삵닏??븍뱜 ??밴쉐
	shared_ptr<CGameObject> gameObjects = std::make_shared<CGameObject>();

	// ?袁⑺뒄 ??쇱젟
	gameObjects->AddComponent(make_shared<Transform>());

	// ??꾨읃????쇱젟
	shared_ptr<Effect> effect = make_shared<Effect>();
	effect->SetEffectInfo(textures, fPassingTime);
	gameObjects->AddComponent(effect);

	// MeshRenderer - ??而??筌롫뗄?? ??용뮞????쇱젟
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();

	meshRenderer->SetMesh(mesh);
	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"BillBoard_Texture");

	shared_ptr<Texture> texture = gameObjects->GetEffect()->GetTexture();
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(shader);
	material->SetTexture(0, texture);
	meshRenderer->SetMaterial(material);

	gameObjects->AddComponent(meshRenderer);

	return gameObjects;
}

std::vector<std::shared_ptr<CGameObject>> Scene_Test::CreateSkillBase(const std::wstring& skillName, const std::wstring& fbxPath)
{
	// ??쎄텢???????fbx ??삵닏??븍뱜 嚥≪뮆諭?
	shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(fbxPath);

	vector<shared_ptr<CGameObject>> gameObjects = meshData->Instantiate();

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetName(skillName);
		gameObject->SetCheckFrustum(false);

		uint32 materialSize = gameObject->GetMeshRenderer()->GetMaterialSize();

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Alpha_Blend_Object");

		for (uint32 i = 0; i < materialSize; ++i)
		{
			gameObject->GetMeshRenderer()->GetMaterial(i)->SetShader(shader);
		}
	}

	return gameObjects;
}

std::shared_ptr<CGameObject> Scene_Test::CreateArtifactBase(vector<shared_ptr<class Texture>> textures)
{
	shared_ptr<CGameObject> gameObjects = std::make_shared<CGameObject>();

	gameObjects->AddComponent(make_shared<Transform>());

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();

	meshRenderer->SetMesh(mesh);
	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"BillBoard_Texture");

	shared_ptr<Texture> texture = textures[0];
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(shader);
	material->SetTexture(0, texture);
	material->SetFloat(2, 1.f);
	meshRenderer->SetMaterial(material);

	gameObjects->AddComponent(meshRenderer);
	gameObjects->SetCheckFrustum(false);

	gameObjects->SetCheckFrustum(false);

	return gameObjects;
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

	if (m_overlappedObjects.contains(id) == true)
		return;

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	float scaleRatio{ packet.Read<float>() };

	int32_t state{ packet.Read<int32_t>() };
	float updateTime{ packet.Read<float>() };

	server::OBJECT_TYPE objType{ packet.Read<server::OBJECT_TYPE>() };
	server::FBX_TYPE fbxType{ packet.Read<server::FBX_TYPE>() };

	ObjectDesc objectDesc;
	ClassifyObject(fbxType, objectDesc, state);

	if (objType == server::OBJECT_TYPE::PLAYER)
		objType = server::OBJECT_TYPE::REMOTE_PLAYER;

	std::vector<std::shared_ptr<CGameObject>> gameObjects{ CreateAnimatedObject(objectDesc) };
	gameObjects = AddNetworkToObject(gameObjects, objType, id);

	for (auto& gameObject : gameObjects)
	{
		Matrix matWorld{ Matrix::CreateScale(scale / scaleRatio) };
		matWorld *= Matrix::CreateTranslation(pos);

		gameObject->GetTransform()->SetWorldMatrix(matWorld);
		gameObject->SetObjectType(objType);

		if (magic_enum::enum_value<Monster_Golem::GOLEM_STATE>(state) == Monster_Golem::GOLEM_STATE::WALK)
			gameObject->GetAnimator()->PlayFrame(state, updateTime, 2.f);
		else if (objType == server::OBJECT_TYPE::BOSS)
			gameObject->GetAnimator()->PlayFrame(state, updateTime);
	}

	AddObjectToScene(objType, gameObjects);
	GET_NETWORK->AddNetworkObject(id, gameObjects);

	m_overlappedObjects.insert(id);

	switch (objType)
	{
		case server::OBJECT_TYPE::REMOTE_PLAYER:
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

	float scaleRatio{ packet.Read<float>() };

	server::OBJECT_TYPE objType{ packet.Read<server::OBJECT_TYPE>() };
	server::FBX_TYPE fbxType{ packet.Read<server::FBX_TYPE>() };

	ObjectDesc objectDesc;
	ClassifyObject(fbxType, objectDesc);

	network::NetworkGameObject gameObjects{ CreateMapObject(objectDesc) };
	gameObjects = AddNetworkToObject(gameObjects, objType, objID);

	for (auto& gameObject : gameObjects)
	{
		Matrix matWorld{ Matrix::CreateScale(scale / scaleRatio) };
		matWorld *= Matrix::CreateFromQuaternion(quat);
		matWorld *= Matrix::CreateTranslation(pos);
		gameObject->GetTransform()->SetWorldMatrix(matWorld);

		if ((magic_enum::enum_integer(server::OBJECT_TYPE::PLAYER_FIREBALL) <= magic_enum::enum_integer(objType)
			and magic_enum::enum_integer(objType) <= magic_enum::enum_integer(server::OBJECT_TYPE::MONSTER_POISONBALL))
			or (objType == server::OBJECT_TYPE::MAP_OBJECT))
		{
			gameObject->AddComponent(objectDesc.script);
		}

		AddObjectEffectScript(gameObject, fbxType);
		gameObject->SetObjectType(objType);
	}

	AddObjectToScene(objType, gameObjects);
	GET_NETWORK->AddNetworkObject(objID, gameObjects);

	m_overlappedObjects.insert(objID);
}

void Scene_Test::ClassifyObject(server::FBX_TYPE type, ObjectDesc& objectDesc, int32_t stateIndex)
{
	objectDesc.vPostion = Vec3(0.f, 0.f, 0.f);
	objectDesc.vScale = Vec3(1.f, 1.f, 1.f);

	switch (type)
	{
#pragma region [PLAYER]
		case server::FBX_TYPE::NANA:
		{
			objectDesc.strName = L"Nana";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Nana\\Nana.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type, stateIndex);
			std::wcout << objectDesc.strName << std::endl;
		}
		break;
		case server::FBX_TYPE::MISTIC:
		{
			objectDesc.strName = L"Mistic";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Mistic\\Mistic.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type, stateIndex);
			std::wcout << objectDesc.strName << std::endl;
		}
		break;
		case server::FBX_TYPE::CARMEL:
		{
			objectDesc.strName = L"Carmel";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Carmel\\Carmel.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type, stateIndex);
			std::wcout << objectDesc.strName << std::endl;
		}
		break;
#pragma endregion
#pragma region [MONSTER]
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
#pragma endregion
#pragma region [SKILL]
		case server::FBX_TYPE::PLAYER_FIREBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>();
			objectDesc.vScale = { 2.5f, 2.5f, 2.5f };
		}
		break;
		case server::FBX_TYPE::PLAYER_ICEBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>();
			objectDesc.vScale = { 2.5f, 2.5f, 2.5f };
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
		case server::FBX_TYPE::PLAYER_METEOR:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Rolling Rock.fbx";
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
			objectDesc.vScale = { 0.3f, 0.3f, 0.3f };
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST3_BALL:
		{
			objectDesc.strName = L"Weeper Cast3";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>();
			objectDesc.vScale = { 2.f, 2.f, 2.f };
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
#pragma endregion
#pragma region [OBJECT]
		case server::FBX_TYPE::SCATTER_ROCK:
		{
			objectDesc.strName = L"Scatter Rock";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Scatter Rock.fbx";
			objectDesc.script = std::make_shared<PhysxObject_Script>();
		}
		break;
		case server::FBX_TYPE::ROLLING_ROCK:
		{
			objectDesc.strName = L"Rolling Rock";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Rolling Rock.fbx";
			objectDesc.script = std::make_shared<PhysxObject_Script>();
		}
		break;
		case server::FBX_TYPE::PILLAR_BRIDGE:
		{
			objectDesc.strName = L"Pillar Bridge";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Pillar Bridge.fbx";
			objectDesc.script = std::make_shared<PhysxObject_Script>();
		}
		break;
#pragma endregion
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
			AddPlayer(gameObjects);
			std::cout << "ADD REMOTE PLAYER" << std::endl;
		}
		break;
		case server::OBJECT_TYPE::MONSTER:
		{
			AddMonster(gameObjects);
			std::cout << "ADD MONSTER" << std::endl;
		}
		break;
		case server::OBJECT_TYPE::BOSS:
		{
			AddBoss(gameObjects);
			std::cout << "ADD BOSS" << std::endl;
		}
		break;
		default:
		{
			AddNetworkObject(gameObjects);
		}
		break;
	}
}

void Scene_Test::AddObjectEffectScript(std::shared_ptr<CGameObject>& gameObject, server::FBX_TYPE type)
{
	switch (type)
	{
		case server::FBX_TYPE::PLAYER_FIREBALL:
		{

		}
		break;
		case server::FBX_TYPE::PLAYER_ICEBALL:
		{

		}
		break;
		case server::FBX_TYPE::PLAYER_POISONBALL:
		{

		}
		break;
		case server::FBX_TYPE::PLAYER_THUNDERBALL:
		{

		}
		break;
		case server::FBX_TYPE::PLAYER_METEOR:
		{

		}
		break;
		case server::FBX_TYPE::WEEPER_CAST1_BALL:
		{
			// ??몿而???
			gameObject->AddComponent(std::make_shared<WeeperSkill1_Script>());
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST2_BALL:
		{
			// ?癒?┛??????
			gameObject->AddComponent(std::make_shared<WeeperSkill2_Script>());
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			// ?臾? ??
			gameObject->AddComponent(std::make_shared<WeeperSkill2Scatter_Script>());
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST3_BALL:
		{
			// ??? ??
			gameObject->AddComponent(std::make_shared<WeeperSkill3_Script>());
		}
		break;
		case server::FBX_TYPE::MONSTER_ICEBALL:
		{
		}
		break;
		case server::FBX_TYPE::MONSTER_THUNDERBALL:
		{
		}
		break;
		case server::FBX_TYPE::MONSTER_POISONBALL:
		{
		}
		break;
		case server::FBX_TYPE::MONSTER_DARKBALL:
		{
		}
		break;
		default:
		break;
	}
}

void Scene_Test::AddEffectTextures()
{
	int32_t index{ 0 };

	EffectInfo effect;
	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Explode", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::EXPLODE] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Fire", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::FIRE] = effect;

#pragma region [HIT]
	effect.index = index++;
	effect.speed = 0.003f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_In_Dispersal", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IN_DISPERSAL] = effect;

	effect.index = index++;
	effect.speed = 0.0045f;
	effect.scale = { 2000.f, 2000.f, 1.f };
	effect.alpha = 0.5f;
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Circle_Wave", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::CIRCLE_WAVE] = effect;

	effect.alpha = 1.f;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Hit", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::HIT] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact1", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT1] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact2", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT2] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact3", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT3] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact4", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT4] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact5", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT5] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact6", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT6] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact7", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT7] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact8", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT8] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact9", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT9] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact10", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT10] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact11", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT11] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact13", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT13] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact14", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT14] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact15", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT15] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact16", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT16] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 400.f, 400.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact17", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT17] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact18", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT18] = effect;

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 800.f, 800.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Impact19", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::IMPACT19] = effect;
#pragma endregion

	effect.index = index++;
	effect.speed = 0.004f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Paralys", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::PARALYS] = effect;

#pragma region [SWORD]
	effect.index = index++;
	effect.speed = 0.0065f;
	effect.scale = { 400.f, 400.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Slash", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::SLASH] = effect;

	effect.index = index++;
	effect.speed = 0.00625f;
	effect.scale = { 400.f, 400.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Slash_Special", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::SLASH_SPECIAL] = effect;

	effect.index = index++;
	effect.speed = 0.0065f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Sword1", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::SWORD1] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Fire_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::FIRE_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Ice_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::ICE_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Lightning_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::LIGHTNING_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Star_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::STAR_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_V_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::V_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Light_Wave_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::LIGHT_WAVE_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Lune_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::LUNE_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Cross_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::CROSS_SWORD] = effect;

	effect.index = index++;
	effect.speed = 0.006f;
	effect.scale = { 500.f, 500.f, 1.f };
	m_billboards.push_back(GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Spell_Sword", effect.speed));
	m_billboardInfo[server::EFFECT_TYPE::SPELL_SWORD] = effect;
#pragma endregion
}

void Scene_Test::RemoveObject(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };
	auto type{ packet.Read<server::OBJECT_TYPE>() };

	switch (type)
	{
		case server::OBJECT_TYPE::REMOTE_PLAYER:
		{
			if (m_overlappedObjects.contains(id) == false)
				return;

			auto player{ GetPlayer() };
			network::NetworkGameObject removeObject;

			for (auto& object : player)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObject.push_back(object);
			}

			RemovePlayer(removeObject);

			std::cout << "REMOVE REMOTE PLAYER" << std::endl;

			GET_NETWORK->RemoveNetworkObject(id);
			m_overlappedObjects.erase(id);
		}
		break;
		case server::OBJECT_TYPE::BOSS:
		{
			if (m_overlappedObjects.contains(id) == false)
				return;

			auto boss{ GetBoss() };
			network::NetworkGameObject removeObject;

			for (auto& object : boss)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObject.push_back(object);
			}

			RemoveBoss(removeObject);

			std::cout << "REMOVE BOSS" << std::endl;

			GET_NETWORK->RemoveNetworkObject(id);
			m_overlappedObjects.erase(id);
		}
		break;
		case server::OBJECT_TYPE::MONSTER:
		{
			if (m_overlappedObjects.contains(id) == false)
				return;

			auto monster{ GetMonster() };
			network::NetworkGameObject removeObject;

			for (auto& object : monster)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObject.push_back(object);
			}

			RemoveMonster(removeObject);

			std::cout << "REMOVE MONSTER" << std::endl;

			GET_NETWORK->RemoveNetworkObject(id);

			m_overlappedObjects.erase(id);
		}
		break;
#pragma region [SKILL]
		case server::OBJECT_TYPE::PLAYER_FIREBALL:
		{
			RemoveNonAnimatedObject(id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Fire Explosion.wav");
		}
		break;
		case server::OBJECT_TYPE::PLAYER_ICEBALL:
		{
			RemoveNonAnimatedObject(id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Ice Hit.wav");
		}
		break;
		case server::OBJECT_TYPE::PLAYER_POISONBALL:
		{
			RemoveNonAnimatedObject(id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"PoisonAcid Hit.wav");
		}
		break;
		case server::OBJECT_TYPE::PLAYER_METEOR:
		{
			if (m_overlappedObjects.contains(id) == false)
				return;

			auto objects{ GetNetworkObject() };
			network::NetworkGameObject removeObjects;

			for (auto& object : objects)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObjects.push_back(object);
			}

			m_overlappedObjects.erase(id);
			RemoveNetworkObject(removeObjects);
			GET_NETWORK->RemoveNetworkObject(id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Fire Explosion.wav");
		}
		break;
		case server::OBJECT_TYPE::PLAYER_THUNDERBALL:
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
#pragma endregion
#pragma region [OBJECT]
		case server::OBJECT_TYPE::MAP_OBJECT:
		case server::OBJECT_TYPE::PHYSX_OBJECT:
#pragma endregion
		{
			if (m_overlappedObjects.contains(id) == false)
				return;

			auto objects{ GetNetworkObject() };
			network::NetworkGameObject removeObjects;

			for (auto& object : objects)
			{
				if (object->GetNetwork()->GetID() == id)
					removeObjects.push_back(object);
			}

			m_overlappedObjects.erase(id);
			RemoveNetworkObject(removeObjects);
			GET_NETWORK->RemoveNetworkObject(id);
		}
		break;
		default:
		break;
	}
}

void Scene_Test::RemoveNonAnimatedObject(int32_t id)
{
	if (m_overlappedObjects.contains(id) == false)
		return;

	auto objects{ GetNetworkObject() };
	network::NetworkGameObject removeObjects;

	auto effect{ m_billboardInfo[server::EFFECT_TYPE::EXPLODE] };

	for (auto& object : objects)
	{
		if (object->GetNetwork()->GetID() == id)
		{
			removeObjects.push_back(object);
			effect.pos = object->GetTransform()->GetWorldPosition();
		}
	}

	m_overlappedObjects.erase(id);
	RemoveNetworkObject(removeObjects);
	GET_NETWORK->RemoveNetworkObject(id);

	GET_SINGLE(EffectManager)->SetBillBoardInfo(m_billboards[effect.index], effect.pos, effect.scale, effect.speed);
	GET_SINGLE(EffectManager)->PlayBillBoard(m_billboards[effect.index]);
}

void Scene_Test::PlayEffect(network::CPacket& packet)
{
	int32_t effectIndex{ packet.Read<int32_t>() };

	Vec3 effectPos{};
	effectPos.x = packet.Read<float>();
	effectPos.y = packet.Read<float>();
	effectPos.z = packet.Read<float>();
	server::EFFECT_TYPE effectType{ magic_enum::enum_value<server::EFFECT_TYPE>(effectIndex) };

	auto info{ m_billboardInfo[effectType] };

	GET_SINGLE(EffectManager)->SetBillBoardInfo(m_billboards[info.index], effectPos, info.scale, info.speed, info.alpha);
	GET_SINGLE(EffectManager)->PlayBillBoard(m_billboards[info.index]);
}

void Scene_Test::ChangeSound(network::CPacket& packet)
{
	server::SOUND_TYPE soundType{ packet.Read<server::SOUND_TYPE>() };

	switch (soundType)
	{
		case server::SOUND_TYPE::BATTLE:
		{
			if (playMusic == true)
			{
				GET_SINGLE(CSoundMgr)->StopBGMSound();
				GET_SINGLE(CSoundMgr)->PlayBGM(L"Battle.ogg");
			}
		}
		break;
		case server::SOUND_TYPE::PUNCH:
		{
			if (playSound == true)
			{
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Punch.wav");
			}
		}
		break;
		case server::SOUND_TYPE::SMASH:
		{
			if (playSound == true)
			{
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Smash.ogg");
			}
		}
		break;
		case server::SOUND_TYPE::ROAR:
		{
			if (playSound == true)
			{
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Roar.mp3");
			}
		}
		break;
		default:
		break;
	}
}

void Scene_Test::TriggerBehaviour(network::CPacket& packet)
{
	if (nullptr == m_fadeScript)
		return;

	auto triggerType{ packet.Read<server::TRIGGER_INTERACTION_TYPE>() };

	switch (triggerType)
	{
		case server::TRIGGER_INTERACTION_TYPE::GUIDE_UI1:
		{
			Print("show guide");
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL1_OUT:
		case server::TRIGGER_INTERACTION_TYPE::PORTAL2_OUT:
		case server::TRIGGER_INTERACTION_TYPE::PORTAL3_OUT:
		case server::TRIGGER_INTERACTION_TYPE::PORTAL4_OUT:
		case server::TRIGGER_INTERACTION_TYPE::PORTAL5_OUT:
		{
			m_fadeScript->FadeOut();
		}
		break;
		default:
		break;
	}
}

void Scene_Test::TriggerInteractionCount(network::CPacket& packet)
{
	auto type{ packet.Read<server::TRIGGER_INTERACTION_TYPE>() };
	int32_t count{ packet.Read<int32_t>() };

	m_portalUIScript->SetCount(count);

	switch (type)
	{
		case server::TRIGGER_INTERACTION_TYPE::PORTAL1_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::FirstBoss);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL2_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::Cave);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL3_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::SecondRoom_Bridge_SecondBoss);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL4_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::ThirdRoom_RockRolling);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL5_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::LastBoss_TreasureRoom);
			}
		}
		break;
		default:
		break;
	}
}

void Scene_Test::PlayCutScene(network::CPacket& packet)
{
	auto sceneType{ packet.Read<server::CUT_SCENE_TYPE>() };

	switch (sceneType)
	{
		// 아티팩트 파괴 후 보호막 사라지는 신
		case server::CUT_SCENE_TYPE::SCENE1:
		{
			m_scenematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));

			for (auto& script : m_artifactMagicScript)
			{
				script->Fade(3.f);
			}

			const_cast<std::vector<std::shared_ptr<CGameObject>>&>(GetMapObjects()).pop_back();
		}
		break;
		// 기둥을 처음 발견해서 기둥의 보호막을 보여주는 신
		case server::CUT_SCENE_TYPE::SCENE2:
		{
			m_scenematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));
			m_oneTimeDialogueScript["PILLAR_HINT"]->StartRender(3.f, 5.f);
		}
		break;
		// 메테오에 기둥이 넘어지는 신
		case server::CUT_SCENE_TYPE::SCENE3:
		{
			m_scenematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));
		}
		break;
		case server::CUT_SCENE_TYPE::SCENE4:
		{

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE5:
		{

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE6:
		{

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE7:
		{

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE8:
		{

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE9:
		{

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE10:
		{

		}
		break;
		default:
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

void Scene_Test::PushMapData(MAP_TYPE eType, std::vector<std::shared_ptr<CGameObject>> objects)
{
	switch (eType)
	{
		case MAP_TYPE::Cave:
		{
			for (auto& object : objects)
				m_splitMap_3.push_back(object);
		}
		break;
		case MAP_TYPE::FirstBoss:
		{
			for (auto& object : objects)
				m_splitMap_2.push_back(object);
		}
		break;
		case MAP_TYPE::LastBoss_TreasureRoom:
		{
			for (auto& object : objects)
				m_splitMap_6.push_back(object);
		}
		break;
		case MAP_TYPE::SecondRoom_Bridge_SecondBoss:
		{
			for (auto& object : objects)
				m_splitMap_4.push_back(object);
		}
		break;
		case MAP_TYPE::StartRoom:
		{
			for (auto& object : objects)
				m_splitMap_1.push_back(object);
		}
		break;
		case MAP_TYPE::ThirdRoom_RockRolling:
		{
			for (auto& object : objects)
				m_splitMap_5.push_back(object);
		}
		break;
		case MAP_TYPE::END:
		break;
		default:
		break;
	}
}

shared_ptr<CScene> Scene_Test::Create(server::FBX_TYPE eType)
{
	shared_ptr<Scene_Test> pInstance = std::make_shared<Scene_Test>();

	pInstance->Init(pInstance, eType);

	return pInstance;
}

void Scene_Test::CheckMapMove(void)
{
	if (nullptr == m_fadeScript)
		return;

	if (m_fadeScript->GetFading())
	{
		m_eNextMapType = m_fadeScript->GetMapType();
		if (m_eNextMapType != m_eCurrentMapType)
		{
			MoveMap(m_eNextMapType);
		}
	}
}

void Scene_Test::MoveMap(MAP_TYPE eType)
{
	if (m_eNextMapType == m_eCurrentMapType)
		return;

	std::vector<std::shared_ptr<CGameObject>> mapObjects;

	switch (eType)
	{
		case MAP_TYPE::StartRoom:
		{
			mapObjects = m_splitMap_1;
		}
		break;
		case MAP_TYPE::FirstBoss:
		{
			mapObjects = m_splitMap_2;
		}
		break;
		case MAP_TYPE::Cave:
		{
			mapObjects = m_splitMap_3;
		}
		break;
		case MAP_TYPE::SecondRoom_Bridge_SecondBoss:
		{
			mapObjects = m_splitMap_4;

			ObjectDesc objectDesc;
			objectDesc.strName = L"Magic Crystal";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Artifact\\Magic Crystal\\Magic Crystal.fbx";
			objectDesc.vPostion = Vec3{ 0.f, 0.f, 0.f };
			objectDesc.vScale = Vec3{ 1.f, 1.f, 1.f };

			network::NetworkGameObject gameObjects{ CreateMapObject(objectDesc) };

			for (auto& gameObject : gameObjects)
			{
				Matrix matWorld{ Matrix::CreateScale(100.f) };
				matWorld *= Matrix::CreateTranslation(-1750.f, -1690.f, 20465.f);
				gameObject->GetTransform()->SetWorldMatrix(matWorld);

				gameObject->SetObjectType(server::OBJECT_TYPE::TRIGGER_OBJECT2);
				mapObjects.push_back(gameObject);
			}
		}
		break;
		case MAP_TYPE::ThirdRoom_RockRolling:
		{
			mapObjects = m_splitMap_5;
		}
		break;
		case MAP_TYPE::LastBoss_TreasureRoom:
		{
			mapObjects = m_splitMap_6;
		}
		break;
		case MAP_TYPE::END:
		break;
		default:
		break;
	}

	ClearMapObject();

	for (auto& mapObject : mapObjects)
	{
		mapObject->SetCheckFrustum(false);
		mapObject->SetStatic(false);
		AddMapObject(mapObject);
	}

	m_eCurrentMapType = eType;
}

void Scene_Test::Init(shared_ptr<Scene_Test> pScene, server::FBX_TYPE eType)
{
	CreateLayer();
	CreateComputeShader();
	CreateMainCamera(pScene);
	CreateUICamera(pScene);
	CreateSkyBox(pScene);
	CreateUI(pScene);
	CreateLights(pScene);
	CreateMap(pScene);
	//CreateBillBoard(pScene);
	//CreateSkill(pScene);

	//CreateEffect(pScene);

	CreatePlayer(pScene, eType);

	AddEffectTextures();

	CreateFade(pScene);

	//CreateMRTUI(pScene);
	CreatePortalUI(pScene);
	CreateMagicArtifactEffect(pScene);
}