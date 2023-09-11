#include "pch.h"
#include "Scene_Test.h"

#pragma region [ENGINE]
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
#include <FontManager.h>
#include <Font.h>
#include <UI.h>
#pragma endregion

#include "Scripts.hpp"
#include "Creator.h"

//#define MOVEMENT

Scene_Test::Scene_Test() :
	m_eCurrentMapType{ MAP_TYPE::END },
	m_eNextMapType{ MAP_TYPE::END },
	m_InfoUIScript{ std::make_shared<InfoUI_Script>() },
	m_fadeScript{ nullptr },
	m_portalUIScript{ nullptr },
	m_cinematicScript{ nullptr },
	m_closeButton{ nullptr },
	m_openSetting{ false },
	m_recvFadeIn{ std::make_shared<bool>(false) },
	m_recvFadeOut{ std::make_shared<bool>(false) },
	m_recvExplosionSkill{ std::make_shared<bool>(false) }
{
	auto pos{ GetRatio(-100.f, 75.f) };
	Vec2 scale{ 100.f };
	pos.x += scale.x / 2.f;

	for (int32_t i = 0; i < 3; ++i)
	{
		m_partyUITransform[i] = { pos, scale };
		pos.y -= 100.f;
	}
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
		GET_SINGLE(CSoundMgr)->StopBGMSound();
		GET_SINGLE(CSoundMgr)->PlayBGM(L"Battle.ogg");
		//GET_SINGLE(CSoundMgr)->PlayBGM(L"World.ogg");
	}
}

void Scene_Test::Update()
{
	__super::Update();

	SendKeyInput();
	CheckMapMove();

	ChangeBossUIVisibility();
	ChangePopUpVisibility();
	ChangeVolume();
	ChangeMuteTexture();
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
			case ProtocolID::WR_MONSTER_HP_ACK:
			{
				ChangeMonsterHP(packet);
			}
			break;
			default:
			break;
		}

		queue.pop_front();
	}

	PopRequestQueue(size);
}

#pragma region [CLIENT]
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

void Scene_Test::CreateMainCamera(std::shared_ptr<CScene> pScene)
{
	shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->SetName(L"Main_Camera");
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45??

#ifdef MOVEMENT
	shared_ptr<Movement_Script> pMovementScript = make_shared<Movement_Script>(7);
	camera->AddComponent(pMovementScript);
#else
	shared_ptr<Camera_Basic> pCameraScript = make_shared<Camera_Basic>();
	m_InfoUIScript->SetCameraScript(pCameraScript);
	camera->AddComponent(pCameraScript);

	shared_ptr<Cinematic_Script> pSenematicScript = make_shared<Cinematic_Script>();
	m_cinematicScript = pSenematicScript;
	pSenematicScript->SetScript(pCameraScript);
	camera->AddComponent(pSenematicScript);
#endif

	camera->GetCamera()->SetFar(30000.f);
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, -500.f));
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI????筌〓씮??
	pScene->AddGameObject(camera);
}

void Scene_Test::CreateUICamera(std::shared_ptr<CScene> pScene)
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

void Scene_Test::CreateSkyBox(std::shared_ptr<CScene> pScene)
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

void Scene_Test::CreateLights(std::shared_ptr<CScene> pScene)
{
	LightDesc lightDesc;
	lightDesc.vDirection = Vec3(0.f, -1.f, 0.f);
	lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
	lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
	lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

	pScene->AddDirectionalLight(lightDesc);
}

void Scene_Test::CreatePlayer(std::shared_ptr<CScene> pScene, server::FBX_TYPE player)
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
		//gameObject->SetName(userName);
	}

	AddPlayer(gameObjects);
	GET_NETWORK->AddNetworkComponent(gameObjects);

	Matrix matWorld = Matrix::CreateTranslation(Vec3(0.f, -1500.f, 0.f));
	for (auto& object : gameObjects)
	{
		object->GetTransform()->SetWorldMatrix(matWorld);
	}
}

void Scene_Test::CreateSphere(std::shared_ptr<CScene> pScene)
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

void Scene_Test::CreateUI(shared_ptr<CScene> pScene, server::FBX_TYPE player)
{
	CreateOneTimeDialogue();
	CreatePlayerUI(player);
	CreatePartyPlayerUI(GET_NETWORK->GetID(), player, GET_NETWORK->GetName());

	CreatePopUp();
}

#pragma region [CLIENT]
void Scene_Test::CreateMRTUI(std::shared_ptr<CScene> pScene)
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

void Scene_Test::CreatePortalUI(std::shared_ptr<CScene> pScene)
{
	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
	m_portalUIScript = std::make_shared<PortalUI_Script>();
	obj->AddComponent(m_portalUIScript);

	pScene->AddGameObject(obj);
}

void Scene_Test::CreateMap(std::shared_ptr<CScene> pScene)
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

	mapLoader.ExtractMapInfo(L"..\\Resources\\FBX\\SplitMap\\Client\\LastBoss_TreasureRoom2.fbx");
	PushMapData(MAP_TYPE::LastBoss_TreasureRoom, mapLoader.GetMapObjectInfo());

	m_eNextMapType = MAP_TYPE::FirstBoss;
	MoveMap(m_eNextMapType);
}

void Scene_Test::CreateSkill(std::shared_ptr<CScene> pScene)
{
	// ??삵닏??븍뱜 ??밴쉐
	std::vector<std::shared_ptr<CGameObject>> gameObjects{ CreateSkillBase(L"BombBase", L"..\\Resources\\FBX\\Skill\\Sphere\\White.fbx") };

	for (auto& object : gameObjects)
	{
		Matrix matWorld{ Matrix::CreateTranslation(Vec3{0.f, -2750.f, 0.f}) };
		object->GetTransform()->SetWorldMatrix(matWorld);

		object->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		object->AddComponent(std::make_shared<Bomb_Script>(2.f));
	}

	// ??삵닏??븍뱜 ?곕떽?
	AddGameObject(gameObjects);
}

void Scene_Test::CreateSkill(const std::wstring& colorName, const Vec3& worldPos, const Vec3& localScale, float alpha, float scaleSpeed)
{
	std::vector<std::shared_ptr<CGameObject>> gameObjects{ CreateSkillBase(colorName, L"..\\Resources\\FBX\\Skill\\Sphere\\" + colorName + L".fbx") };

	for (auto& object : gameObjects)
	{
		//Matrix matWorld{ object->GetTransform()->GetWorldMatrix() };
		//matWorld.Translation(worldPos);
		Matrix matWorld{ Matrix::CreateTranslation(worldPos) };
		object->GetTransform()->SetWorldMatrix(matWorld);

		object->GetTransform()->SetLocalScale(localScale);

		auto script{ std::make_shared<Bomb_Script>(scaleSpeed, alpha) };
		script->SetRecvFlag(m_recvExplosionSkill);
		object->AddComponent(script);
		m_skillObject.push_back(script);
	}

	AddGameObject(gameObjects);
}

void Scene_Test::CreateBillBoard(std::shared_ptr<CScene> pScene)
{
	// ??용뮞????밴쉐
	vector<shared_ptr<Texture>> textures = GET_SINGLE(Resources)->LoadTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Sprite\\Fire.png", 64);

	// ??삵닏??븍뱜 ??밴쉐
	shared_ptr<CGameObject> gameObjects = CreateBillBoardBase(textures, 0.0001f);

	gameObjects->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));

	// ??삵닏??븍뱜 ?곕떽?
	pScene->AddGameObject(gameObjects);
}

void Scene_Test::CreateEffect(std::shared_ptr<CScene> pScene)
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

void Scene_Test::CreateMagicArtifactEffect(std::shared_ptr<CScene> pScene)
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

std::shared_ptr<CGameObject> Scene_Test::CreateBillBoardBase(std::vector<shared_ptr<Texture>> textures, float fPassingTime)
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

std::shared_ptr<CGameObject> Scene_Test::CreateEffectBase(std::vector<shared_ptr<Texture>> textures, float fPassingTime)
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
	std::shared_ptr<MeshData> meshData{ GET_SINGLE(Resources)->LoadFBX(fbxPath) };
	std::vector<std::shared_ptr<CGameObject>> gameObjects{ meshData->Instantiate() };

	for (auto& gameObject : gameObjects)
	{
		gameObject->SetName(skillName);
		gameObject->SetCheckFrustum(false);

		std::shared_ptr<Shader> shader{ GET_SHADER(L"Alpha_Blend_Object") };
		uint32_t materialSize{ gameObject->GetMeshRenderer()->GetMaterialSize() };

		for (uint32_t i = 0; i < materialSize; ++i)
		{
			gameObject->GetMeshRenderer()->GetMaterial(i)->SetShader(shader);
		}
	}

	return gameObjects;
}

std::shared_ptr<CGameObject> Scene_Test::CreateArtifactBase(std::vector<shared_ptr<Texture>> textures)
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

void Scene_Test::CreateFade(std::shared_ptr<CScene> pScene)
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
	fade_script->SetFade(m_recvFadeIn, m_recvFadeOut);
	gameObject->AddComponent(fade_script);

	m_fadeScript = fade_script;

	pScene->AddGameObject(gameObject);
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
#pragma endregion

#pragma region [UI]
void Scene_Test::CreatePlayerUI(server::FBX_TYPE character)
{
	CreatePlayerImage(character);
	//UITransform hpTransform{};
	//float mpYPos{ CreatePlayerMPBar() };
	//CreatePlayerHPBar(mpYPos, hpTransform);
	//CreatePlayerImage(character, hpTransform);
}

void Scene_Test::CreatePlayerImage(server::FBX_TYPE character)
{
	std::shared_ptr<Texture> texture{ nullptr };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	if (character == server::FBX_TYPE::NANA)
		texture = GET_TEXTURE(L"Nana_In Game");
	else if (character == server::FBX_TYPE::MISTIC)
		texture = GET_TEXTURE(L"Mistic_In Game");
	else
		texture = GET_TEXTURE(L"Carmel_In Game");

	std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	// 위치 및 카메라 인덱스 설정
	Vec2 pos{ GetRatio(-20.f, -100.f) };
	pos.y += 150.f / 2.f;

	auto transform{ obj->GetTransform() };
	transform->SetLocalScale(Vec3{ 150.f, 150.f, 1.f });
	transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

	AddGameObject(obj);
}

void Scene_Test::CreatePlayerImage(server::FBX_TYPE character, UITransform& hpTransform)
{
	std::shared_ptr<Texture> texture{ nullptr };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	if (character == server::FBX_TYPE::NANA)
		texture = GET_TEXTURE(L"Nana_In Game");
	else if (character == server::FBX_TYPE::MISTIC)
		texture = GET_TEXTURE(L"Mistic_In Game");
	else
		texture = GET_TEXTURE(L"Carmel_In Game");

	std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	// 위치 및 카메라 인덱스 설정
	Vec2 pos{};
	pos.x = hpTransform.pos.x - (hpTransform.scale.x - 150.f) / 2.f;
	pos.y = hpTransform.pos.y + (hpTransform.scale.y + 150.f) / 2.f;
	auto transform{ obj->GetTransform() };

	transform->SetLocalScale(Vec3{ 150.f, 150.f, 1.f });
	transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

	AddGameObject(obj);
}

void Scene_Test::CreatePlayerName(const std::wstring& name)
{
	GET_SINGLE(FontManager)->RenderFonts(name, GetRatio(0.f, 0.f), Vec2{ 10.f, 10.f });
}

void Scene_Test::CreatePlayerHPBar(float yPos, UITransform& hpTransform)
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Player Slider Frame(C)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	Vec3 leftScale{ 38.f * 13.f, 62.f / 2.f, 1.f };
	Vec2 pos{ GetRatio(-23.f, -100.f) };
	pos.y = yPos + leftScale.y;
	hpTransform = { pos, Vec2{ leftScale.x, leftScale.y } };

	// FRAME
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}

	// FILL
	leftScale = { 10.f * 49.f, 52.f * 0.5f, 1.f };
	texture = GET_TEXTURE(L"HP(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y - 1.f, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}
}

float Scene_Test::CreatePlayerMPBar()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Player Slider Frame(C)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	Vec3 leftScale{ 38.f * 13.f, 62.f / 2.f, 1.f };
	Vec2 pos{ GetRatio(-23.f, -100.f) };
	pos.y += leftScale.y / 2.f;

	// FRAME
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}

	// FILL
	leftScale = { 10.f * 49.f, 52.f * 0.5f, 1.f };
	texture = GET_TEXTURE(L"MP(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y - 1.f, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}

	return pos.y;
}

void Scene_Test::CreatePartyPlayerUI(int32_t id, server::FBX_TYPE character, const std::wstring& name)
{
	CreatePartyPlayerImage(character, m_partyUITransform[id]);
	//auto mpPos{ CreatePartyPlayerMPBar(m_partyUITransform[id]) };
	//CreatePartyPlayerHPBar(mpPos, m_partyUITransform[id]);
}

void Scene_Test::CreatePartyPlayerImage(server::FBX_TYPE character, UITransform trans)
{
	std::shared_ptr<Texture> texture{ nullptr };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	if (character == server::FBX_TYPE::NANA)
		texture = GET_TEXTURE(L"Nana_In Game");
	else if (character == server::FBX_TYPE::MISTIC)
		texture = GET_TEXTURE(L"Mistic_In Game");
	else
		texture = GET_TEXTURE(L"Carmel_In Game");

	std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	// 위치 및 카메라 인덱스 설정

	auto transform{ obj->GetTransform() };
	transform->SetLocalScale(Vec3{ trans.scale.x, trans.scale.y, 1.f });
	transform->SetLocalPosition(Vec3{ trans.pos.x, trans.pos.y, 100.f });

	AddGameObject(obj);
}

void Scene_Test::CreatePartyPlayerName(const std::wstring& name)
{
	GET_SINGLE(FontManager)->RenderFonts(name, GetRatio(0.f, 0.f), Vec2{ 10.f, 10.f });
}

void Scene_Test::CreatePartyPlayerHPBar(float yPos, UITransform trans)
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Player Slider Frame(L)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	Vec3 scale{ 38.f * 5.f, 62.f * 0.5f, 1.f };
	Vec2 pos{};
	pos.x = trans.pos.x + (scale.x + trans.scale.x) / 2.f;
	pos.y = yPos + scale.y;

	// FRAME
	texture = GET_TEXTURE(L"Player Slider Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}

	// FILL
	texture = GET_TEXTURE(L"HP(C)");
	scale = { 10.f * 18.5f, 52.f * 0.5f, 1.f };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}
}

float Scene_Test::CreatePartyPlayerMPBar(UITransform trans)
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Player Slider Frame(L)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	Vec3 scale{ 38.f * 5.f, 62.f * 0.5f, 1.f };
	Vec2 pos{};
	pos.x = trans.pos.x + (scale.x + trans.scale.x) / 2.f;
	pos.y = trans.pos.y - (trans.scale.y - scale.y) / 2.f;
	float yPos{ pos.y };

	// FRAME
	texture = GET_TEXTURE(L"Player Slider Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}

	// FILL
	texture = GET_TEXTURE(L"MP(C)");
	scale = { 10.f * 18.5f, 52.f * 0.5f, 1.f };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		//std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		//script->InsertTextures(texture);
		//obj->AddComponent(script);

		AddGameObject(obj);
	}

	return yPos;
}

void Scene_Test::CreateBossUI(server::FBX_TYPE boss, int32_t hp)
{
	UITransform transform{ GetRatio(0.f, 100.f), Vec2{} };
	CreateBossHPBar(transform, boss, hp);
	CreateBossClassIcon(transform, boss);
}

void Scene_Test::CreateBossHPBar(UITransform& trans, server::FBX_TYPE boss, int32_t hp)
{
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Player Slider Frame(C)") };

	trans.scale = { 38.f * 18.f, 62.f * 0.8f };
	trans.pos.y -= 62.f;

	// FRAME
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(Vec3{ trans.scale.x, trans.scale.y, 1.f });
		transform->SetLocalPosition(Vec3{ trans.pos.x, trans.pos.y, 400.f });

		obj->GetUI()->SetVisible(false);

		AddGameObject(obj);

		if (boss == server::FBX_TYPE::WEEPER1)
			m_weeperUIObjets.push_back(obj);
		else
			m_golemUIObjets.push_back(obj);
	}

	// FILL
	texture = GET_TEXTURE(L"HP(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec3 scale{ 10.f * 67.7f, 52.f * 0.8f, 1.f };
		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ trans.pos.x, trans.pos.y, 400.f });

		std::shared_ptr<BossHP_Script> script{ std::make_shared<BossHP_Script>(hp) };
		script->InsertTextures(texture);
		obj->AddComponent(script);
		script->Start();

		obj->GetUI()->SetVisible(false);

		AddGameObject(obj);

		if (boss == server::FBX_TYPE::WEEPER1)
		{
			m_weeperHPScript = script;
			m_weeperUIObjets.push_back(obj);
		}
		else
		{
			m_golemHPScript = script;
			m_golemUIObjets.push_back(obj);
		}
	}
}

void Scene_Test::CreateBossClassIcon(UITransform& trans, server::FBX_TYPE boss)
{
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };
	std::shared_ptr<Texture> texture{ nullptr };

	switch (boss)
	{
		case server::FBX_TYPE::WEEPER1:
		case server::FBX_TYPE::WEEPER2:
		case server::FBX_TYPE::WEEPER3:
		case server::FBX_TYPE::WEEPER4:
		case server::FBX_TYPE::WEEPER5:
		case server::FBX_TYPE::WEEPER6:
		case server::FBX_TYPE::WEEPER7:
		case server::FBX_TYPE::WEEPER_EMISSIVE:
		{
			texture = GET_TEXTURE(L"Weeper Class");
		}
		break;
		case server::FBX_TYPE::BLUE_GOLEM:
		case server::FBX_TYPE::RED_GOLEM:
		case server::FBX_TYPE::GREEN_GOLEM:
		{
			texture = GET_TEXTURE(L"Golem Class");
		}
		break;
		default:
		break;
	}

	std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	Vec3 scale{ 256.f * 0.5f };
	trans.pos.x += (trans.scale.x + scale.x) / 2.f;

	auto transform{ obj->GetTransform() };
	transform->SetLocalScale(scale);
	transform->SetLocalPosition(Vec3{ trans.pos.x, trans.pos.y, 100.f });

	obj->GetUI()->SetVisible(false);

	AddGameObject(obj);

	if (boss == server::FBX_TYPE::WEEPER1)
		m_weeperUIObjets.push_back(obj);
	else
		m_golemUIObjets.push_back(obj);
}

void Scene_Test::CreateOneTimeDialogue()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Pillar Hint") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto pos{ GetRatio(0.f, 50.f) };
		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(Vec3{ 916.f, 106.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

		std::shared_ptr<OneTimeDialogue_Script> behaviour{ std::make_shared<OneTimeDialogue_Script>("PILLAR_HINT") };
		behaviour->InsertTextures(texture);
		m_oneTimeDialogueScript["PILLAR_HINT"] = behaviour;

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}

	texture = GET_TEXTURE(L"Pillar Hint2");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto pos{ GetRatio(0.f, 50.f) };
		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(Vec3{ 916.f, 106.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

		std::shared_ptr<OneTimeDialogue_Script> behaviour{ std::make_shared<OneTimeDialogue_Script>("PILLAR_HINT2") };
		behaviour->InsertTextures(texture);
		m_oneTimeDialogueScript["PILLAR_HINT2"] = behaviour;

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}

	texture = GET_TEXTURE(L"Weeper Hint");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto pos{ GetRatio(0.f, 50.f) };
		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(Vec3{ 916.f, 106.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

		std::shared_ptr<OneTimeDialogue_Script> behaviour{ std::make_shared<OneTimeDialogue_Script>("WEEPER_HINT") };
		behaviour->InsertTextures(texture);
		m_oneTimeDialogueScript["WEEPER_HINT"] = behaviour;

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}

	texture = GET_TEXTURE(L"Golem Hint");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto pos{ GetRatio(0.f, 50.f) };
		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(Vec3{ 916.f, 106.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

		std::shared_ptr<OneTimeDialogue_Script> behaviour{ std::make_shared<OneTimeDialogue_Script>("GOLEM_HINT") };
		behaviour->InsertTextures(texture);
		m_oneTimeDialogueScript["GOLEM_HINT"] = behaviour;

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}
}

void Scene_Test::CreatePopUp()
{
	CreateBlur();
	CreateCloseButton();
	CreateSettingFrame();
	CreateBGMButton();
	CreateBGMSlider();
	CreateSEButton();
	CreateSESlider();
}

void Scene_Test::ChangeBossUIVisibility()
{
	if (m_eNextMapType == MAP_TYPE::FirstBoss)
	{
		for (auto& obj : m_weeperUIObjets)
		{
			obj->GetUI()->SetVisible(true);
		}
	}
	else if (m_eNextMapType == MAP_TYPE::LastBoss_TreasureRoom)
	{
		for (auto& obj : m_golemUIObjets)
		{
			obj->GetUI()->SetVisible(true);
		}
	}
	else
	{
		for (auto& obj : m_weeperUIObjets)
		{
			obj->GetUI()->SetVisible(false);
		}

		for (auto& obj : m_golemUIObjets)
		{
			obj->GetUI()->SetVisible(false);
		}
	}
}
#pragma endregion

#pragma region [POP UP]
void Scene_Test::CreateBlur()
{
	std::shared_ptr<Texture> texture{ GET_RESOURCE->GetBlurTexture() };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

	auto transform{ obj->GetTransform() };
	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	transform->SetLocalScale(Vec3{ width, height, 1.f });
	transform->SetLocalPosition(Vec3{ 0.f, 0.f, 450.f });

	obj->GetMeshRenderer()->GetMaterial()->SetFloat(2, 1.f);

	AddGameObject(obj);
	GET_SINGLE(SceneManager)->SetBlurUI(obj);
}

void Scene_Test::CreateCloseButton()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Close") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-93.f, 86.f) };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 0.5f };
#endif

		transform->SetLocalScale(Vec3{ 118.f * ratio, 118.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Close Selected"));
		obj->AddComponent(script);
		m_closeButton = script;

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Test::CreateSettingFrame()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Setting Frame") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		auto transform{ obj->GetTransform() };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 0.5f };
#endif

		transform->SetLocalScale(Vec3{ 898.f * ratio, 1029.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ 0.f, 0.f, 400.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Test::CreateBGMButton()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"BGM") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	// BGM BUTTON
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-33.f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 45.f * ratio, 60.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<MuteButton_Script> script{ std::make_shared<MuteButton_Script>(MuteButton_Script::SOUND_TYPE::BGM) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"BGM Mute"));
		obj->AddComponent(script);
		m_muteButton.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Test::CreateBGMSlider()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Slider Frame(L)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

#pragma region [FRAME]
	// SLIDER FRAME(L)
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-22.9f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(R)
	texture = GET_TEXTURE(L"Slider Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(34.85f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(C)
	texture = GET_TEXTURE(L"Slider Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 23.f * ratio * 14.37f, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [INNER FRAME]
	// SLIDER INNER FRAME(L)
	texture = GET_TEXTURE(L"Slider Inner Frame(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.5f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(R)
	texture = GET_TEXTURE(L"Slider Inner Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.35f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(C)
	texture = GET_TEXTURE(L"Slider Inner Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 8.f * ratio * 41.5f, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL AREA]
	// SLIDER FILL AREA(L)
	texture = GET_TEXTURE(L"Slider Fill Area(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(R)
	texture = GET_TEXTURE(L"Slider Fill Area(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(C)
	texture = GET_TEXTURE(L"Slider Fill Area(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 6.f * ratio * 55.7f, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL]
	// SLIDER FILL(L)
	texture = GET_TEXTURE(L"Slider Fill(L)");

#ifdef RELEASE
	float ratio{ 1.5f };
#else
	float ratio{ 1.f };
#endif

	Vec3 leftScale{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f };
	Vec2 leftPos{ GetRatio(-20.8f, 21.3f) };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ leftPos.x, leftPos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>(false) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(L) Mute"));
		obj->AddComponent(script);
		m_volumeSliderLeftTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL(C)
	texture = GET_TEXTURE(L"Slider Fill(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec3 scale{ 6.f * ratio * 27.5f, 18.f * ratio * 1.5f, 1.f };
		Vec2 pos{ leftPos.x + leftScale.x * 0.5f + scale.x * 0.5f, leftPos.y };

		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(C) Mute"));
		obj->AddComponent(script);
		m_volumeSlider.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

	// SLIDER TIP
	texture = GET_TEXTURE(L"Slider Tip");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(6.f, 20.7f) };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 96.f * ratio, 96.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<SliderTip_Script> script{ std::make_shared<SliderTip_Script>(SliderTip_Script::SLIDER_TYPE::BGM) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Tip Mute"));
		obj->AddComponent(script);
		m_sliderTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Test::CreateSEButton()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"SE") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	// BGM BUTTON
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-33.f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 45.f * ratio, 60.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<MuteButton_Script> script{ std::make_shared<MuteButton_Script>(MuteButton_Script::SOUND_TYPE::SE) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"SE Mute"));
		obj->AddComponent(script);
		m_muteButton.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Test::CreateSESlider()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Slider Frame(L)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

#pragma region [FRAME]
	// SLIDER FRAME(L)
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-22.9f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(R)
	texture = GET_TEXTURE(L"Slider Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(34.85f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(C)
	texture = GET_TEXTURE(L"Slider Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 23.f * ratio * 14.37f, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [INNER FRAME]
	// SLIDER INNER FRAME(L)
	texture = GET_TEXTURE(L"Slider Inner Frame(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.5f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(R)
	texture = GET_TEXTURE(L"Slider Inner Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.35f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(C)
	texture = GET_TEXTURE(L"Slider Inner Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 8.f * ratio * 41.5f, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL AREA]
	// SLIDER FILL AREA(L)
	texture = GET_TEXTURE(L"Slider Fill Area(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(R)
	texture = GET_TEXTURE(L"Slider Fill Area(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(C)
	texture = GET_TEXTURE(L"Slider Fill Area(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 6.f * ratio * 55.7f, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL]
	// SLIDER FILL(L)
	texture = GET_TEXTURE(L"Slider Fill(L)");

#ifdef RELEASE
	float ratio{ 1.5f };
#else
	float ratio{ 1.f };
#endif

	Vec3 leftScale{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f };
	Vec2 leftPos{ GetRatio(-20.8f, -18.7f) };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ leftPos.x, leftPos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>(false) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(L) Mute"));
		obj->AddComponent(script);
		m_volumeSliderLeftTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL(C)
	texture = GET_TEXTURE(L"Slider Fill(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		Vec3 scale{ 6.f * ratio * 27.5f, 18.f * ratio * 1.5f, 1.f };
		Vec2 pos{ leftPos.x + leftScale.x * 0.5f + scale.x * 0.5f, leftPos.y };

		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(C) Mute"));
		obj->AddComponent(script);
		m_volumeSlider.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

	// SLIDER TIP
	texture = GET_TEXTURE(L"Slider Tip");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(6.f, -19.3f) };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 96.f * ratio, 96.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<SliderTip_Script> script{ std::make_shared<SliderTip_Script>(SliderTip_Script::SLIDER_TYPE::SE) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Tip Mute"));
		obj->AddComponent(script);
		m_sliderTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Test::ChangePopUpVisibility()
{
	if (GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::ESC) == true)
	{
		m_openSetting = !m_openSetting;

		if (m_openSetting == true)
		{
			for (auto& obj : m_popUp)
			{
				obj->GetUI()->SetVisible(true);
			}
		}
		else
		{
			for (auto& obj : m_popUp)
			{
				obj->GetUI()->SetVisible(false);
			}
		}
	}

	if (m_closeButton->ClosePopUp() == true)
	{
		for (auto& obj : m_popUp)
		{
			obj->GetUI()->SetVisible(false);
		}

		m_closeButton->SetClosePopUpFlag(false);
		m_openSetting = false;
	}
}

void Scene_Test::ChangeVolume()
{
	if (m_sliderTip[BGM]->GetClick() == true)
		m_volumeSlider[BGM]->SetActive(true);
	else
		m_volumeSlider[BGM]->SetActive(false);

	if (m_sliderTip[SE]->GetClick() == true)
		m_volumeSlider[SE]->SetActive(true);
	else
		m_volumeSlider[SE]->SetActive(false);
}

void Scene_Test::ChangeMuteTexture()
{
	if (m_muteButton[BGM]->IsMute() == true)
	{
		m_sliderTip[BGM]->ChangeTexture(1);
		m_volumeSlider[BGM]->ChangeMuteTexture(true);
		m_volumeSliderLeftTip[BGM]->ChangeMuteTexture(true);
	}
	else
	{
		m_sliderTip[BGM]->ChangeTexture(0);
		m_volumeSlider[BGM]->ChangeMuteTexture(false);
		m_volumeSliderLeftTip[BGM]->ChangeMuteTexture(false);
	}

	if (m_muteButton[SE]->IsMute() == true)
	{
		m_sliderTip[SE]->ChangeTexture(1);
		m_volumeSlider[SE]->ChangeMuteTexture(true);
		m_volumeSliderLeftTip[SE]->ChangeMuteTexture(true);
	}
	else
	{
		m_sliderTip[SE]->ChangeTexture(0);
		m_volumeSlider[SE]->ChangeMuteTexture(false);
		m_volumeSliderLeftTip[SE]->ChangeMuteTexture(false);
	}
}
#pragma endregion

#pragma region [NETWORK]
std::vector<std::shared_ptr<CGameObject>> Scene_Test::AddNetworkToObject(std::vector<std::shared_ptr<CGameObject>> objects, server::OBJECT_TYPE objectType, int32_t id)
{
	std::shared_ptr<network::CNetwork> networkComponent{ std::make_shared<network::CNetwork>(objectType, id) };

	for (auto& gameObject : objects)
	{
		gameObject->AddComponent(networkComponent);
	}

	return objects;
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

	std::wstring name{};
	packet.ReadWString(name);

	int32_t state{ packet.Read<int32_t>() };
	float updateTime{ packet.Read<float>() };

	server::OBJECT_TYPE objType{ packet.Read<server::OBJECT_TYPE>() };
	server::FBX_TYPE fbxType{ packet.Read<server::FBX_TYPE>() };

	int32_t hp{};
	if (objType == server::OBJECT_TYPE::BOSS or objType == server::OBJECT_TYPE::MONSTER)
		hp = packet.Read<int32_t>();

	ObjectDesc objectDesc;
	ClassifyObject(fbxType, objectDesc, state);

	if (objType == server::OBJECT_TYPE::PLAYER)
		objType = server::OBJECT_TYPE::REMOTE_PLAYER;

	objectDesc.strName = name;

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
		{
			CreatePartyPlayerUI(id, fbxType, name);
			objectDesc.script->Start();
		}
		break;
		case server::OBJECT_TYPE::MONSTER:
		case server::OBJECT_TYPE::BOSS:
		{
			CreateBossUI(fbxType, hp);
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

	Quat quat;
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

		//AddObjectEffectScript(gameObject, fbxType);
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
		}
		break;
		case server::FBX_TYPE::MISTIC:
		{
			objectDesc.strName = L"Mistic";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Mistic\\Mistic.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type, stateIndex);
		}
		break;
		case server::FBX_TYPE::CARMEL:
		{
			objectDesc.strName = L"Carmel";
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\Carmel\\Carmel.fbx";
			objectDesc.script = std::make_shared<Player_Script>(type, stateIndex);
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
			objectDesc.strPath = L"..\\Resources\\FBX\\Character\\MoltenGolem\\Red Golem.fbx";
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
			objectDesc.strName = L"Fireball";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Fireball.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>(type, m_fireballEffectCurrentIndex++);
			objectDesc.vScale = Vec3{ 100.f };

			if (m_fireballEffectCurrentIndex == m_fireballEffectStartIndex + 5)
				m_fireballEffectCurrentIndex = m_fireballEffectStartIndex;
		}
		break;
		case server::FBX_TYPE::PLAYER_ICEBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>(type, m_spiralEffectCurrentIndex++);
			objectDesc.vScale = Vec3{ 2.5f };

			if (m_spiralEffectCurrentIndex == m_spiralEffectStartIndex + 5)
				m_spiralEffectCurrentIndex = m_spiralEffectStartIndex;
		}
		break;
		case server::FBX_TYPE::PLAYER_THUNDERBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Stone Sphere.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>(type, m_electricDarkGrayEffectCurrentIndex);
			objectDesc.vScale = Vec3{ 40.f };

			if (m_electricDarkGrayEffectCurrentIndex == m_electricDarkGrayEffectStartIndex + 3)
				m_electricDarkGrayEffectCurrentIndex = m_electricDarkGrayEffectStartIndex;
		}
		break;
		case server::FBX_TYPE::PLAYER_POISONBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>(type);
		}
		break;
		case server::FBX_TYPE::PLAYER_METEOR:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Rolling Rock.fbx";
			objectDesc.script = std::make_shared<PlayerRangeAttack>(type);
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST1_BALL:
		{
			objectDesc.strName = L"Weeper Cast1";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type, m_wpDarkBlueEffectCurrentIndex++);
			//objectDesc.vScale = Vec3{ 70.f };

			if (m_wpDarkBlueEffectCurrentIndex == m_wpDarkBlueEffectStartIndex + 3)
				m_wpDarkBlueEffectCurrentIndex = m_wpDarkBlueEffectStartIndex;
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST2_BALL:
		case server::FBX_TYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			objectDesc.strName = L"Weeper Cast2";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Fireball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type, m_fireballEffectCurrentIndex++);
			objectDesc.vScale = Vec3{ 70.f };

			if (m_fireballEffectCurrentIndex == m_fireballEffectStartIndex + 52)
				m_fireballEffectCurrentIndex = m_fireballEffectStartIndex;
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST3_BALL:
		{
			objectDesc.strName = L"Weeper Cast3";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type, m_spiralEffectCurrentIndex++);
			objectDesc.vScale = { 2.f, 2.f, 2.f };

			if (m_spiralEffectCurrentIndex == m_spiralEffectStartIndex + 5)
				m_spiralEffectCurrentIndex = m_spiralEffectStartIndex;
		}
		break;
		case server::FBX_TYPE::WEEPER_CAST4_BALL:
		{
			objectDesc.strName = L"Weeper Cast4";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Stone Bullet2.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type);
		}
		break;
		case server::FBX_TYPE::MONSTER_ICEBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type);
		}
		break;
		case server::FBX_TYPE::MONSTER_THUNDERBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Stone Spike.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type);
		}
		break;
		case server::FBX_TYPE::MONSTER_POISONBALL:
		{
			objectDesc.strName = L"Sphere";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type);
		}
		break;
		case server::FBX_TYPE::MONSTER_DARKBALL:
		{
			objectDesc.strName = L"Stone Bullet2";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx";
			objectDesc.script = std::make_shared<MonsterRangeAttack>(type);
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
		case server::FBX_TYPE::LAST_BOSS_ROCK:
		{
			objectDesc.strName = L"Last Boss Rock";
			objectDesc.strPath = L"..\\Resources\\FBX\\Models\\Scatter Rock.fbx";
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

void Scene_Test::AddEffectTextures()
{
	EffectInfo effect;
	effect.speed = 0.003f;
	effect.scale = Vec3{ 500.f };

	for (int32_t i = 0; i < 70; ++i)
	{
		effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Explode", effect.speed);

		if (i == 0)
		{
			m_explodeEffectCurrentIndex = effect.index;
			m_explodeEffectStartIndex = effect.index;
			m_billboardInfo[server::EFFECT_TYPE::EXPLODE] = effect;
		}
	}

	effect.speed = 0.003f;
	effect.scale = Vec3{ 500.f };

	for (int32_t i = 0; i < 5; ++i)
	{
		effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Ice_Explode", effect.speed);

		if (i == 0)
		{
			m_iceExplodeEffectStartIndex = effect.index;
			m_iceExplodeEffectCurrentIndex = effect.index;
			m_billboardInfo[server::EFFECT_TYPE::ICE_EXPLODE] = effect;
		}
	}

	effect.speed = 0.003f;
	effect.scale = Vec3{ 300.f };

	for (int32_t i = 0; i < 52; ++i)
	{
		effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Fire", effect.speed);

		if (i == 0)
		{
			m_fireballEffectStartIndex = effect.index;
			m_fireballEffectCurrentIndex = effect.index;
			m_billboardInfo[server::EFFECT_TYPE::FIRE] = effect;
		}
	}

	effect.speed = 0.003f;
	effect.scale = Vec3{ 100.f };

	for (int32_t i = 0; i < 2; ++i)
	{
		effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Shield_Electric_Dark_Gray", effect.speed);

		if (i == 0)
		{
			m_electricDarkGrayEffectStartIndex = effect.index;
			m_electricDarkGrayEffectCurrentIndex = effect.index;
			m_billboardInfo[server::EFFECT_TYPE::SHIELD_ELECTRIC_DARK_GRAY] = effect;
		}
	}

	effect.speed = 0.003f;
	effect.scale = Vec3{ 100.f };

	for (int32_t i = 0; i < 3; ++i)
	{
		effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Whirl_Pool_Dark_Blue", effect.speed);

		if (i == 0)
		{
			m_wpDarkBlueEffectCurrentIndex = effect.index;
			m_wpDarkBlueEffectStartIndex = effect.index;
			m_billboardInfo[server::EFFECT_TYPE::SHIELD_ELECTRIC_DARK_GRAY] = effect;
		}
	}

#pragma region [HIT]
	effect.speed = 0.003f;
	effect.scale = Vec3{ 1000.f };

	for (int32_t i = 0; i < 5; ++i)
	{
		effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_In_Dispersal", effect.speed);

		if (i == 0)
		{
			m_hitEffectStartIndex = effect.index;
			m_hitEffectCurrentIndex = effect.index;
			m_billboardInfo[server::EFFECT_TYPE::IN_DISPERSAL] = effect;
		}
	}

	effect.speed = 0.0045f;
	effect.scale = { 2000.f, 2000.f, 1.f };
	effect.alpha = 0.5f;
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Circle_Wave", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::CIRCLE_WAVE] = effect;

	effect.alpha = 1.f;
#pragma endregion

	effect.speed = 0.004f;
	effect.scale = Vec3{ 500.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Paralys", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::PARALYS] = effect;

#pragma region [SWORD]
	effect.speed = 0.00625f;
	effect.scale = Vec3{ 300.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Slash_Special", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::SLASH_SPECIAL] = effect;

	effect.speed = 0.006f;
	effect.scale = Vec3{ 300.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Fire_Sword", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::FIRE_SWORD] = effect;

	effect.speed = 0.0035f;
	effect.scale = Vec3{ 300.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Ice_Sword", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::ICE_SWORD] = effect;

	effect.speed = 0.0035f;
	effect.scale = Vec3{ 300.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Lightning_Sword", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::LIGHTNING_SWORD] = effect;

	effect.speed = 0.006f;
	effect.scale = Vec3{ 300.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Star_Sword", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::STAR_SWORD] = effect;

	effect.speed = 0.0035f;
	effect.scale = Vec3{ 300.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Lune_Sword", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::LUNE_SWORD] = effect;

	effect.speed = 0.0035f;
	effect.scale = Vec3{ 300.f };
	effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Cross_Sword", effect.speed);
	m_billboardInfo[server::EFFECT_TYPE::CROSS_SWORD] = effect;
#pragma endregion

	effect.speed = 0.003f;
	effect.scale = Vec3{ 300.f };

	for (int32_t i = 0; i < 5; ++i)
	{
		effect.index = GET_SINGLE(EffectManager)->CreateBillBoard(L"Effect_Spiral", effect.speed);

		if (i == 0)
		{
			m_spiralEffectStartIndex = effect.index;
			m_spiralEffectCurrentIndex = effect.index;
			m_billboardInfo[server::EFFECT_TYPE::SPIRAL] = effect;
		}
	}

	effect.speed = 0.003f;
	effect.scale = Vec3{ 500.f };
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

			//m_bossHPScript.reset();
			//
			//RemoveGameObject(m_weeperUIObjets);
			//m_weeperUIObjets.clear();
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
			RemoveNonAnimatedObject(type, id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Fire Explosion.wav");
		}
		break;
		case server::OBJECT_TYPE::PLAYER_ICEBALL:
		{
			RemoveNonAnimatedObject(type, id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Ice Hit.wav");
		}
		break;
		case server::OBJECT_TYPE::PLAYER_THUNDERBALL:
		{
			RemoveNonAnimatedObject(type, id);

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
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Light Explosion.wav");
		}
		break;
		case server::OBJECT_TYPE::WEEPER_CAST1_BALL:
		{
			RemoveNonAnimatedObject(type, id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"PoisonAcid Hit.wav");
		}
		break;
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL:
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			RemoveNonAnimatedObject(type, id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Fire Explosion.wav");
		}
		break;
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL_NUCLEAR:
		{
			RemoveNonAnimatedObject(type, id);
		}
		break;
		case server::OBJECT_TYPE::WEEPER_CAST3_BALL:
		{
			RemoveNonAnimatedObject(type, id);

			if (playSound == true)
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Ice Hit.wav");
		}
		break;
		case server::OBJECT_TYPE::PLAYER_POISONBALL:
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

void Scene_Test::RemoveNonAnimatedObject(server::OBJECT_TYPE type, int32_t id)
{
	if (m_overlappedObjects.contains(id) == false)
		return;

	auto objects{ GetNetworkObject() };
	network::NetworkGameObject removeObjects;

	EffectInfo effect{};

	switch (type)
	{
		case server::OBJECT_TYPE::WEEPER_CAST1_BALL:
		case server::OBJECT_TYPE::PLAYER_FIREBALL:
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			effect = m_billboardInfo[server::EFFECT_TYPE::EXPLODE];
		}
		break;
		case server::OBJECT_TYPE::PLAYER_ICEBALL:
		case server::OBJECT_TYPE::WEEPER_CAST3_BALL:
		{
			effect = m_billboardInfo[server::EFFECT_TYPE::ICE_EXPLODE];
		}
		break;
		default:
		break;
	}

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

	switch (type)
	{
		case server::OBJECT_TYPE::PLAYER_FIREBALL:
		case server::OBJECT_TYPE::WEEPER_CAST1_BALL:
		{
			GET_SINGLE(EffectManager)->SetBillBoardInfo(m_explodeEffectCurrentIndex, effect.pos, effect.scale, effect.speed);
			GET_SINGLE(EffectManager)->PlayBillBoard(m_explodeEffectCurrentIndex++);

			if (m_explodeEffectCurrentIndex == m_explodeEffectStartIndex + 70)
				m_explodeEffectCurrentIndex = m_explodeEffectStartIndex;
		}
		break;
		case server::OBJECT_TYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			GET_SINGLE(EffectManager)->SetBillBoardInfo(m_explodeEffectCurrentIndex, effect.pos, Vec3{ 700.f }, effect.speed);
			GET_SINGLE(EffectManager)->PlayBillBoard(m_explodeEffectCurrentIndex++);

			if (m_explodeEffectCurrentIndex == m_explodeEffectStartIndex + 70)
				m_explodeEffectCurrentIndex = m_explodeEffectStartIndex;
		}
		break;
		case server::OBJECT_TYPE::PLAYER_ICEBALL:
		case server::OBJECT_TYPE::WEEPER_CAST3_BALL:
		{
			GET_SINGLE(EffectManager)->SetBillBoardInfo(m_iceExplodeEffectCurrentIndex, effect.pos, effect.scale, effect.speed);
			GET_SINGLE(EffectManager)->PlayBillBoard(m_iceExplodeEffectCurrentIndex++);

			if (m_iceExplodeEffectCurrentIndex == m_iceExplodeEffectStartIndex + 5)
				m_iceExplodeEffectCurrentIndex = m_iceExplodeEffectStartIndex;
		}
		break;
		default:
		break;
	}
}

void Scene_Test::PlayEffect(network::CPacket& packet)
{
	int32_t effectIndex{ packet.Read<int32_t>() };

	Vec3 effectPos{};
	effectPos.x = packet.Read<float>();
	effectPos.y = packet.Read<float>();
	effectPos.z = packet.Read<float>();
	server::EFFECT_TYPE effectType{ magic_enum::enum_value<server::EFFECT_TYPE>(effectIndex) };

	if (effectType == server::EFFECT_TYPE::NUCLEAR_EXPLOSION)
	{
		if (*m_recvExplosionSkill == true)
			return;

		*m_recvExplosionSkill = true;

		effectPos.y -= 150.f;
		CreateSkill(L"Yellow", effectPos, Vec3{ 150.f }, 1.f, 1.f);
		CreateSkill(L"White", effectPos, Vec3{ 110.f }, 1.f, 2.f);
		CreateSkill(L"Red", effectPos, Vec3{ 70.f }, 1.f, 3.5f);

		return;
	}
	else if (effectType == server::EFFECT_TYPE::SPELL_EXPLOSION)
	{
		if (*m_recvExplosionSkill == true)
			return;

		*m_recvExplosionSkill = true;

		int32_t id{ packet.ReadID() };
		auto bossObjects{ GetBoss() };
		std::shared_ptr<CGameObject> boss{ nullptr };

		for (auto& obj : bossObjects)
		{
			if (obj->GetNetwork()->GetID() == id)
			{
				boss = obj;
				break;
			}
		}

		auto look{ boss->GetTransform()->GetLook() };
		look.Normalize();
		effectPos = boss->GetTransform()->GetWorldPosition();
		effectPos -= look * 500.f;
		effectPos.y -= 50.f;

		CreateSkill(L"Yellow", effectPos, Vec3{ 150.f }, 1.f, 1.f);
		CreateSkill(L"White", effectPos, Vec3{ 110.f }, 1.f, 2.f);
		CreateSkill(L"Red", effectPos, Vec3{ 70.f }, 1.f, 3.5f);

		return;
	}

	auto info{ m_billboardInfo[effectType] };

	if (server::EFFECT_TYPE::SWORD1 <= effectType and effectType <= server::EFFECT_TYPE::SPELL_SWORD)
	{
		effectPos.y -= 50.f;
	}

	if (effectType == server::EFFECT_TYPE::IN_DISPERSAL)
	{
		effectPos.y -= 150.f;

		GET_SINGLE(EffectManager)->SetBillBoardInfo(m_hitEffectCurrentIndex, effectPos, info.scale, info.speed, info.alpha);
		GET_SINGLE(EffectManager)->PlayBillBoard(m_hitEffectCurrentIndex++);

		if (m_hitEffectCurrentIndex == m_hitEffectStartIndex + 5)
			m_hitEffectCurrentIndex = m_hitEffectStartIndex;
	}
	else
	{
		GET_SINGLE(EffectManager)->SetBillBoardInfo(info.index, effectPos, info.scale, info.speed, info.alpha);
		GET_SINGLE(EffectManager)->PlayBillBoard(info.index);
	}
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
		case server::SOUND_TYPE::LIGHT_EXPLOSION:
		{
			if (playSound == true)
			{
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Light Explosion.wav");
			}
		}
		break;
		case server::SOUND_TYPE::NUCLEAR_EXPLOSION:
		{
			if (playSound == true)
			{
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Nuclear Explosion.ogg");
			}
		}
		break;
		case server::SOUND_TYPE::SPELL_EXPLOSION:
		{
			if (playSound == true)
			{
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Spell Explosion.ogg");
			}
		}
		break;
		case server::SOUND_TYPE::LANDING:
		{
			if (playSound == true)
			{
				GET_SINGLE(CSoundMgr)->PlayEffect(L"Landing.wav");
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
			if (m_oneTimeDialogueScript["PILLAR_HINT2"]->IsRendering() == true)
				return;

			m_oneTimeDialogueScript["PILLAR_HINT2"]->StartRender(1.f, 2.f);
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::GUIDE_UI2:
		{
			if (m_oneTimeDialogueScript["WEEPER_HINT"]->IsRendering() == true)
				return;

			m_oneTimeDialogueScript["WEEPER_HINT"]->StartRender(1.f, 2.f);
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::GUIDE_UI3:
		{
			if (m_oneTimeDialogueScript["GOLEM_HINT"]->IsRendering() == true)
				return;

			m_oneTimeDialogueScript["GOLEM_HINT"]->StartRender(1.f, 2.f);
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL1_OUT:
		{
			if (*m_recvFadeOut == true)
				return;

			*m_recvFadeOut = true;

			m_fadeScript->FadeOut();

			if (playMusic == true)
			{
				GET_SINGLE(CSoundMgr)->StopBGMSound();
				GET_SINGLE(CSoundMgr)->PlayBGM(L"Battle.ogg");
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL2_OUT:
		{
			if (*m_recvFadeOut == true)
				return;

			*m_recvFadeOut = true;

			m_fadeScript->FadeOut();

			if (playMusic == true)
			{
				GET_SINGLE(CSoundMgr)->StopBGMSound();
				GET_SINGLE(CSoundMgr)->PlayBGM(L"World.ogg");
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL3_OUT:
		case server::TRIGGER_INTERACTION_TYPE::PORTAL4_OUT:
		{
			if (*m_recvFadeOut == true)
				return;

			*m_recvFadeOut = true;

			m_fadeScript->FadeOut();
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL5_OUT:
		{
			if (*m_recvFadeOut == true)
				return;

			*m_recvFadeOut = true;

			m_fadeScript->FadeOut();

			if (playMusic == true)
			{
				GET_SINGLE(CSoundMgr)->StopBGMSound();
				GET_SINGLE(CSoundMgr)->PlayBGM(L"Battle.ogg");
			}
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
				if (*m_recvFadeIn == true)
					return;

				*m_recvFadeIn = true;

				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::FirstBoss);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL2_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				if (*m_recvFadeIn == true)
					return;

				*m_recvFadeIn = true;

				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::Cave);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL3_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				if (*m_recvFadeIn == true)
					return;

				*m_recvFadeIn = true;

				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::SecondRoom_Bridge_SecondBoss);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL4_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				if (*m_recvFadeIn == true)
					return;

				*m_recvFadeIn = true;

				m_fadeScript->FadeIn();
				m_fadeScript->SetMapType(MAP_TYPE::ThirdRoom_RockRolling);
			}
		}
		break;
		case server::TRIGGER_INTERACTION_TYPE::PORTAL5_IN:
		{
			if (m_portalUIScript->GetCount() == 3)
			{
				if (*m_recvFadeIn == true)
					return;

				*m_recvFadeIn = true;

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
			if (m_cinematicScript->IsPlaying() == true)
				return;

			m_cinematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));

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
			if (m_cinematicScript->IsPlaying() == true)
				return;

			m_cinematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));
			m_oneTimeDialogueScript["PILLAR_HINT"]->StartRender(3.f, 5.f);
		}
		break;
		// 메테오에 기둥이 넘어지는 신
		case server::CUT_SCENE_TYPE::SCENE3:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

			m_cinematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));
		}
		break;
		// 돌덩이가 굴러가서 벽돌 벽을 부수는 신
		case server::CUT_SCENE_TYPE::SCENE4:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

			m_cinematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));
		}
		break;
		// Weeper 등장 컷신
		case server::CUT_SCENE_TYPE::SCENE5:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

			m_cinematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));
		}
		break;
		// Golem 등장 컷신
		case server::CUT_SCENE_TYPE::SCENE6:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

			m_cinematicScript->PlayCinematic(magic_enum::enum_integer(sceneType));
		}
		break;
		case server::CUT_SCENE_TYPE::SCENE7:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE8:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE9:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

		}
		break;
		case server::CUT_SCENE_TYPE::SCENE10:
		{
			if (m_cinematicScript->IsPlaying() == true)
				return;

		}
		break;
		default:
		break;
	}
}

void Scene_Test::ChangeMonsterHP(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };
	auto type{ packet.Read<server::FBX_TYPE>() };
	int32_t hp{ packet.Read<int32_t>() };

	if (type == server::FBX_TYPE::WEEPER1)
	{
		m_weeperHPScript->SetHP(hp);
	}
	else if (type == server::FBX_TYPE::BLUE_GOLEM)
	{
		m_golemHPScript->SetHP(hp);
	}
}
#pragma endregion

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

void Scene_Test::Init(std::shared_ptr<Scene_Test> pScene, server::FBX_TYPE eType)
{
	CreateLayer();
	CreateComputeShader();
	CreateMainCamera(pScene);
	CreateUICamera(pScene);
	CreateSkyBox(pScene);
	CreateUI(pScene, eType);
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

std::shared_ptr<CScene> Scene_Test::Create(server::FBX_TYPE eType)
{
	shared_ptr<Scene_Test> pInstance = std::make_shared<Scene_Test>();

	pInstance->Init(pInstance, eType);

	return pInstance;
}