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
	m_progress = 0;

	// 폰트
	Loading_GamePlayLevel_Font();

	m_progress = 25;

	// 텍스쳐
	Loading_GamePlayLevel_Texture();

	m_progress = 50;

	// 셰이더
	Loading_GamePlayLevel_Shader();

	m_progress = 75;

	// fbx 모델
	Loading_GamePlayLevel_Fbx();

	m_progress = 100;
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Font()
{
	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Texture()
{
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_BrightFlare_DarkGray", L"..\\Resources\\Texture\\Effect\\Bright Flare\\dark gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_BrightFlare_DarkGrayHued", L"..\\Resources\\Texture\\Effect\\Bright Flare\\dark gray hued\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_BrightFlare_Gray", L"..\\Resources\\Texture\\Effect\\Bright Flare\\gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_BrightFlare_Infinity", L"..\\Resources\\Texture\\Effect\\Bright Flare\\Infinity\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_BrightFlare_PurpleHued", L"..\\Resources\\Texture\\Effect\\Bright Flare\\purple hued\\tile.png", 32);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_CircleFrame_DarkBlue", L"..\\Resources\\Texture\\Effect\\Circle Frame\\dark blue\\tile.png", 40);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_CircleFrame_DarkGray", L"..\\Resources\\Texture\\Effect\\Circle Frame\\dark gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_CircleFrame_Gray", L"..\\Resources\\Texture\\Effect\\Circle Frame\\gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_CircleFrame_Purple", L"..\\Resources\\Texture\\Effect\\Circle Frame\\purple\\tile.png", 40);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_CircleFrame_Red", L"..\\Resources\\Texture\\Effect\\Circle Frame\\red\\tile.png", 40);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_CircleFrame_Yellow", L"..\\Resources\\Texture\\Effect\\Circle Frame\\yellow\\tile.png", 32);
	
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Explode", L"..\\Resources\\Texture\\Effect\\Explode\\tile.png", 20);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Explosion_Star_Particle", L"..\\Resources\\Texture\\Effect\\Explosion Star Particle\\tile.png", 15);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Fire\\Fire.png", 64);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_Dark", L"..\\Resources\\Texture\\Effect\\Flash In\\dark\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_DarkBlue", L"..\\Resources\\Texture\\Effect\\Flash In\\dark blue\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_Green", L"..\\Resources\\Texture\\Effect\\Flash In\\green\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_LightBlue", L"..\\Resources\\Texture\\Effect\\Flash In\\light blue\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_Red", L"..\\Resources\\Texture\\Effect\\Flash In\\red\\tile.png", 34);
	
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_DarkBlue", L"..\\Resources\\Texture\\Effect\\Implode\\dark blue\\tile.png", 29);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_DarkGray", L"..\\Resources\\Texture\\Effect\\Implode\\dark gray\\tile.png", 29);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_Gray", L"..\\Resources\\Texture\\Effect\\Implode\\gray\\tile.png", 29);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_Green", L"..\\Resources\\Texture\\Effect\\Implode\\green\\tile.png", 29);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_OrangeBrown", L"..\\Resources\\Texture\\Effect\\Implode\\orange brown\\tile.png", 29);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_Purple", L"..\\Resources\\Texture\\Effect\\Implode\\purple\\tile.png", 29);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_Red", L"..\\Resources\\Texture\\Effect\\Implode\\red\\tile.png", 29);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_Yellow", L"..\\Resources\\Texture\\Effect\\Implode\\yellow\\tile.png", 29);
	
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implosion_Gray", L"..\\Resources\\Texture\\Effect\\Implosion\\gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implosion_Green", L"..\\Resources\\Texture\\Effect\\Implosion\\green\\tile.png", 32);
	
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_In_Dispersal", L"..\\Resources\\Texture\\Effect\\In Dispersal\\tile.png", 33);
	
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_In_StarBurst_Gray", L"..\\Resources\\Texture\\Effect\\In Star Burst\\gray\\tile.png", 40);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_In_StarBurst_Infinity", L"..\\Resources\\Texture\\Effect\\In Star Burst\\infinity\\tile.png", 40);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Shield_Electric_DarkGray", L"..\\Resources\\Texture\\Effect\\Shield Electric\\dark gray\\tile.png", 30);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Shield_Electric_Green", L"..\\Resources\\Texture\\Effect\\Shield Electric\\green\\tile.png", 30);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Smokey_Flicker_Flare_Dark", L"..\\Resources\\Texture\\Effect\\Smokey Flicker Flare\\dark\\tile.png", 48);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Smokey_Swirl", L"..\\Resources\\Texture\\Effect\\smokey swirl\\tile.png", 48);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball\\tile.png", 49);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball_Small", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball_small\\tile.png", 49);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball_Small_Wind", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball_small_wind\\tile.png", 49);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball_Wind", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball_wind\\tile.png", 49);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Star_Out_Area_Elipse_Dark", L"..\\Resources\\Texture\\Effect\\Star Out Area Elipse\\dark\\tile.png", 40);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Star_Out_Area_Elipse_Red", L"..\\Resources\\Texture\\Effect\\Star Out Area Elipse\\red\\tile.png", 40);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Teleport_Circle", L"..\\Resources\\Texture\\Effect\\teleport circle\\tile.png", 16);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Torch_Dripping_DarkBlue", L"..\\Resources\\Texture\\Effect\\Torch Dripping\\dark blue\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Torch_Dripping_DarkGray", L"..\\Resources\\Texture\\Effect\\Torch Dripping\\dark gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Torch_Dripping_Green", L"..\\Resources\\Texture\\Effect\\Torch Dripping\\green\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Torch_Dripping_OrangeBrown", L"..\\Resources\\Texture\\Effect\\Torch Dripping\\orange brown\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Torch_Dripping_Purple", L"..\\Resources\\Texture\\Effect\\Torch Dripping\\purple\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Torch_Dripping_Red", L"..\\Resources\\Texture\\Effect\\Torch Dripping\\red\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Torch_Dripping_Yellow", L"..\\Resources\\Texture\\Effect\\Torch Dripping\\yellow\\tile.png", 32);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Whirl_Pool_DarkBlue", L"..\\Resources\\Texture\\Effect\\whirl pool\\dark blue\\tile.png", 36);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Whirl_Pool_DarkGray", L"..\\Resources\\Texture\\Effect\\whirl pool\\dark gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Whirl_Pool_Gray", L"..\\Resources\\Texture\\Effect\\whirl pool\\gray\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Whirl_Pool_Green", L"..\\Resources\\Texture\\Effect\\whirl pool\\green\\tile.png", 32);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Whirl_Pool_Yellow", L"..\\Resources\\Texture\\Effect\\whirl pool\\yellow\\tile.png", 32);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Zap_Spiral_Gray", L"..\\Resources\\Texture\\Effect\\Zap Spiral\\gray\\tile.png", 48);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Zap_Spiral_Green", L"..\\Resources\\Texture\\Effect\\Zap Spiral\\green\\tile.png", 48);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Zap_Spiral_Yellow", L"..\\Resources\\Texture\\Effect\\Zap Spiral\\yellow\\tile.png", 48);
		
	GET_SINGLE(Resources)->Load<Texture>(L"Bar", L"..\\Resources\\Texture\\In Game\\bar.png");
	GET_SINGLE(Resources)->Load<Texture>(L"HP", L"..\\Resources\\Texture\\In Game\\hp.png");
	GET_SINGLE(Resources)->Load<Texture>(L"MP", L"..\\Resources\\Texture\\In Game\\mp.png");

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
	m_progress = 0;

	Loading_ForLobby_Font();

	m_progress = 50;

	Loading_ForLobby_Texture();

	m_progress = 100;
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

	m_progress = 50;

	Loading_ForCharacterSelection_Texture();

	m_progress = 100;
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
