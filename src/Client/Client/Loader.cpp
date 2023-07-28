#include "pch.h"
#include "Loader.h"
#include "Resources.h"
#include "MeshData.h"
#include "Engine.h"

shared_ptr<CLoader> CLoader::Create(SCENE eScene)
{
	shared_ptr<CLoader> pInstance = make_shared<CLoader>();

	if (FAILED(pInstance->Loading(eScene)))
	{
		MSG_BOX(TEXT("Failed to Created CLoader"));
	}

	return pInstance;
}

unsigned int APIENTRY ThreadEntryFunc(CLoader* pArg)
{
	//shared_ptr<CLoader> pLoader = static_cast<shared_ptr<CLoader>>(pArg);

	//shared_ptr<CLoader> pLoader = pArg;

	CLoader* pLoader = (CLoader*)pArg;

	EnterCriticalSection(pLoader->Get_CriticalSection());

	switch (pLoader->Get_SceneIndex())
	{
		case SCENE_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
		case SCENE_LOBBY:
		pLoader->Loading_ForLobby();
		break;
		case SCENE_CHARACTER_SELECT:
		pLoader->Loading_ForCharacterSelection();
		break;
	}

	LeaveCriticalSection(pLoader->Get_CriticalSection());

	return 0;
}

CLoader::CLoader(void)
{
}

HRESULT CLoader::Loading(SCENE eScene)
{
	m_eScene = eScene;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	InitializeCriticalSection(&m_CriticalSection);

	thread loaderThread = std::thread{ ThreadEntryFunc, this };

	loaderThread.join();

	//m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc, this, 0, nullptr);
	//if (0 == m_hThread)
	//	return E_FAIL;

	return S_OK;
}

#pragma region [GAME PLAY LEVEL]
HRESULT CLoader::Loading_ForGamePlayLevel()
{
	// 여기서 리소스 추가

	m_isFinished = false;

	// 폰트
	Loading_GamePlayLevel_Font();

	// 텍스쳐
	Loading_GamePlayLevel_Texture();

	// 셰이더
	Loading_GamePlayLevel_Shader();

	// fbx 모델
	Loading_GamePlayLevel_Fbx();

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Font()
{
	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Texture()
{
	GET_SINGLE(Resources)->LoadTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Fire\\Fire.png", 64);

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Shader()
{
	// ComputeShader 생성
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

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Fbx()
{
	// fbx 모델
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Mistic\\Mistic.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Nana\\Nana.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Carmel\\Carmel.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Weeper\\Weeper.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\MoltenGolem\\Blue Golem.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Black Scorpion.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\GimmicksRAW.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Models.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Models2.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Pillar Bridge.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Rolling Rock.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Scatter Rock.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone Bullet1.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone Bullet2.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone.fbx");

	return S_OK;
}
#pragma endregion

#pragma region [LOBBY]
HRESULT CLoader::Loading_ForLobby()
{
	// 여기서 리소스 추가

	m_isFinished = false;

	Loading_ForLobby_Font();
	Loading_ForLobby_Texture();

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForLobby_Font()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForLobby_Texture()
{
	GET_SINGLE(Resources)->Load<Texture>(L"Lobby_InGame", L"..\\Resources\\Texture\\Lobby\\Lobby_InGame.PNG");
	GET_SINGLE(Resources)->Load<Texture>(L"NewStart", L"..\\Resources\\Texture\\Lobby\\NewStart.png");
	GET_SINGLE(Resources)->Load<Texture>(L"NewStart_Pressed", L"..\\Resources\\Texture\\Lobby\\NewStart_Pressed.png");

	return S_OK;
}
#pragma endregion

#pragma region [CHARACTER SELECTION]
HRESULT CLoader::Loading_ForCharacterSelection()
{
	m_isFinished = false;

	Loading_ForCharacterSelection_Font();
	Loading_ForCharacterSelection_Texture();

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForCharacterSelection_Font()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForCharacterSelection_Texture()
{
	GET_SINGLE(Resources)->Load<Texture>(L"Nana", L"..\\Resources\\Texture\\Character Select\\Nana.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Mistic", L"..\\Resources\\Texture\\Character Select\\Mistic.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Carmel", L"..\\Resources\\Texture\\Character Select\\Carmel.png");

	GET_SINGLE(Resources)->Load<Texture>(L"Nana_Selected", L"..\\Resources\\Texture\\Character Select\\Nana_Selected.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Mistic_Selected", L"..\\Resources\\Texture\\Character Select\\Mistic_Selected.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Carmel_Selected", L"..\\Resources\\Texture\\Character Select\\Carmel_Selected.png");

	return S_OK;
}
#pragma endregion
