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
		case SCENE_START:
		pLoader->Loading_ForStart();
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
	// ?ш린??由ъ냼??異붽?

	m_isFinished = false;
	m_progress = 0;

	// ?고듃
	Loading_GamePlayLevel_Font();

	m_progress = 25;

	// ?띿뒪爾?
	Loading_GamePlayLevel_Texture();

	m_progress = 50;

	// ?곗씠??
	Loading_GamePlayLevel_Shader();

	m_progress = 75;

	// fbx 紐⑤뜽
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
	Loading_GamePlayLevel_UI();
	Loading_GamePlayLevel_Effect();

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Shader()
{
	// ComputeShader ?앹꽦
	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

	// UAV ??Texture ?앹꽦
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
		DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
	material->SetShader(shader);
	material->SetInt(0, 1);
	GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

	// ?곕젅??洹몃９ (1 * 1024 * 1)
	material->Dispatch(1, 1024, 1);

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Fbx()
{
	// fbx 紐⑤뜽
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

HRESULT CLoader::Loading_GamePlayLevel_UI()
{
	LOAD_TEXTURE(L"Bar", L"..\\Resources\\Texture\\UI\\In Game\\bar.png");
	LOAD_TEXTURE(L"HP", L"..\\Resources\\Texture\\UI\\In Game\\hp.png");
	LOAD_TEXTURE(L"MP", L"..\\Resources\\Texture\\UI\\In Game\\mp.png");

	LOAD_TEXTURE(L"Pillar Hint", L"..\\Resources\\Texture\\UI\\Font\\Pillar Hint.png");

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Effect()
{
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Artifact_Protection", L"..\\Resources\\Texture\\Effect\\Artifact Protection\\tile.png", 16);

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

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Circle_Wave", L"..\\Resources\\Texture\\Effect\\Circle Wave\\tile.png", 6);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Explode", L"..\\Resources\\Texture\\Effect\\Explode\\tile.png", 20);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Explosion_Star_Particle", L"..\\Resources\\Texture\\Effect\\Explosion Star Particle\\tile.png", 15);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Fire\\Fire.png", 64);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_Dark", L"..\\Resources\\Texture\\Effect\\Flash In\\dark\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_DarkBlue", L"..\\Resources\\Texture\\Effect\\Flash In\\dark blue\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_Green", L"..\\Resources\\Texture\\Effect\\Flash In\\green\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_LightBlue", L"..\\Resources\\Texture\\Effect\\Flash In\\light blue\\tile.png", 34);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_Red", L"..\\Resources\\Texture\\Effect\\Flash In\\red\\tile.png", 34);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Hit", L"..\\Resources\\Texture\\Effect\\Hit\\tile.png", 4);

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

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact1", L"..\\Resources\\Texture\\Effect\\Impact1\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact2", L"..\\Resources\\Texture\\Effect\\Impact2\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact3", L"..\\Resources\\Texture\\Effect\\Impact3\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact4", L"..\\Resources\\Texture\\Effect\\Impact4\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact5", L"..\\Resources\\Texture\\Effect\\Impact5\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact6", L"..\\Resources\\Texture\\Effect\\Impact6\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact7", L"..\\Resources\\Texture\\Effect\\Impact7\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact8", L"..\\Resources\\Texture\\Effect\\Impact8\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact9", L"..\\Resources\\Texture\\Effect\\Impact9\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact10", L"..\\Resources\\Texture\\Effect\\Impact10\\tile.png", 4);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact11", L"..\\Resources\\Texture\\Effect\\Impact11\\tile.png", 4);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact13", L"..\\Resources\\Texture\\Effect\\Impact13\\tile.png", 5);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact14", L"..\\Resources\\Texture\\Effect\\Impact14\\tile.png", 5);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact15", L"..\\Resources\\Texture\\Effect\\Impact15\\tile.png", 5);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact16", L"..\\Resources\\Texture\\Effect\\Impact16\\tile.png", 5);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact17", L"..\\Resources\\Texture\\Effect\\Impact17\\tile.png", 5);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact18", L"..\\Resources\\Texture\\Effect\\Impact18\\tile.png", 5);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Impact19", L"..\\Resources\\Texture\\Effect\\Impact19\\tile.png", 5);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_In_Dispersal", L"..\\Resources\\Texture\\Effect\\In Dispersal\\tile.png", 33);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_In_StarBurst_Gray", L"..\\Resources\\Texture\\Effect\\In Star Burst\\gray\\tile.png", 40);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_In_StarBurst_Infinity", L"..\\Resources\\Texture\\Effect\\In Star Burst\\infinity\\tile.png", 40);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Paralys", L"..\\Resources\\Texture\\Effect\\Paralys\\tile.png", 21);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Shield_Electric_DarkGray", L"..\\Resources\\Texture\\Effect\\Shield Electric\\dark gray\\tile.png", 30);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Shield_Electric_Green", L"..\\Resources\\Texture\\Effect\\Shield Electric\\green\\tile.png", 30);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Slash", L"..\\Resources\\Texture\\Effect\\Slash\\tile.png", 5);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Slash_Special", L"..\\Resources\\Texture\\Effect\\Slash Special\\tile.png", 18);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Smokey_Flicker_Flare_Dark", L"..\\Resources\\Texture\\Effect\\Smokey Flicker Flare\\dark\\tile.png", 48);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Smokey_Swirl", L"..\\Resources\\Texture\\Effect\\smokey swirl\\tile.png", 48);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball\\tile.png", 49);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball_Small", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball_small\\tile.png", 49);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball_Small_Wind", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball_small_wind\\tile.png", 49);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sparkling_Fireball_Wind", L"..\\Resources\\Texture\\Effect\\sparkling fireball\\particles_fireball_wind\\tile.png", 49);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Star_Out_Area_Elipse_Dark", L"..\\Resources\\Texture\\Effect\\Star Out Area Elipse\\dark\\tile.png", 40);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Star_Out_Area_Elipse_Red", L"..\\Resources\\Texture\\Effect\\Star Out Area Elipse\\red\\tile.png", 40);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Sword1", L"..\\Resources\\Texture\\Effect\\Sword1\\tile.png", 6);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Fire_Sword", L"..\\Resources\\Texture\\Effect\\Fire Sword\\tile.png", 14);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Ice_Sword", L"..\\Resources\\Texture\\Effect\\Ice Sword\\tile.png", 13);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Lightning_Sword", L"..\\Resources\\Texture\\Effect\\Lightning Sword\\tile.png", 14);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Star_Sword", L"..\\Resources\\Texture\\Effect\\Star Sword\\tile.png", 24);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_V_Sword", L"..\\Resources\\Texture\\Effect\\V Sword\\tile.png", 16);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Light_Wave_Sword", L"..\\Resources\\Texture\\Effect\\Light Wave Sword\\tile.png", 16);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Lune_Sword", L"..\\Resources\\Texture\\Effect\\Lune Sword\\tile.png", 28);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Cross_Sword", L"..\\Resources\\Texture\\Effect\\Cross Sword\\tile.png", 19);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Spell_Sword", L"..\\Resources\\Texture\\Effect\\Spell Sword\\tile.png", 25);

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

	return S_OK;
}
#pragma endregion

#pragma region [LOBBY]
HRESULT CLoader::Loading_ForStart()
{
	// ?ш린??由ъ냼??異붽?

	m_isFinished = false;
	m_progress = 0;

	Loading_ForStart_Font();

	m_progress = 50;

	Loading_ForStart_Texture();

	m_progress = 100;
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStart_Font()
{
	return S_OK;
}

HRESULT CLoader::Loading_ForStart_Texture()
{
	LOAD_TEXTURE(L"Lobby_InGame", L"..\\Resources\\Texture\\UI\\Start\\Lobby_InGame.PNG");

	LOAD_TEXTURE(L"Title", L"..\\Resources\\Texture\\UI\\Font\\Title.png");
	LOAD_TEXTURE(L"Log In", L"..\\Resources\\Texture\\UI\\Font\\Log In.png");

	LOAD_TEXTURE(L"Button", L"..\\Resources\\Texture\\UI\\Button\\Button.png");
	LOAD_TEXTURE(L"Button Selected", L"..\\Resources\\Texture\\UI\\Button\\Button_selected.png");
	LOAD_TEXTURE(L"Setting", L"..\\Resources\\Texture\\UI\\Button\\Setting.png");

	LOAD_TEXTURE(L"White Blur", L"..\\Resources\\Texture\\UI\\Blur\\White.png");
	LOAD_TEXTURE(L"Setting Frame", L"..\\Resources\\Texture\\UI\\Frame\\Setting Frame.png");

	LOAD_TEXTURE(L"BGM", L"..\\Resources\\Texture\\UI\\Icon\\BGM.png");
	LOAD_TEXTURE(L"BGM Mute", L"..\\Resources\\Texture\\UI\\Icon\\BGM_mute.png");
	LOAD_TEXTURE(L"SE", L"..\\Resources\\Texture\\UI\\Icon\\SE.png");
	LOAD_TEXTURE(L"SE Mute", L"..\\Resources\\Texture\\UI\\Icon\\SE_mute.png");
	LOAD_TEXTURE(L"Close", L"..\\Resources\\Texture\\UI\\Button\\Close.png");
	LOAD_TEXTURE(L"Close Selected", L"..\\Resources\\Texture\\UI\\Button\\Close_selected.png");

	LOAD_TEXTURE(L"Slider Frame(L)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_Frame(L).png");
	LOAD_TEXTURE(L"Slider Frame(C)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_Frame(C).png");
	LOAD_TEXTURE(L"Slider Frame(R)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_Frame(R).png");

	LOAD_TEXTURE(L"Slider Inner Frame(L)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_InnerFrame(L).png");
	LOAD_TEXTURE(L"Slider Inner Frame(C)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_InnerFrame(C).png");
	LOAD_TEXTURE(L"Slider Inner Frame(R)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_InnerFrame(R).png");

	LOAD_TEXTURE(L"Slider Fill Area(L)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_FillArea(L).png");
	LOAD_TEXTURE(L"Slider Fill Area(C)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_FillArea(C).png");
	LOAD_TEXTURE(L"Slider Fill Area(R)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_FillArea(R).png");

	LOAD_TEXTURE(L"Slider Fill(L)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_Fill(L).png");
	LOAD_TEXTURE(L"Slider Fill(C)", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_Fill(C).png");
	LOAD_TEXTURE(L"Slider Fill(L) Mute", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_Fill(L)_mute.png");
	LOAD_TEXTURE(L"Slider Fill(C) Mute", L"..\\Resources\\Texture\\UI\\Slider\\Slider01_Fill(C)_mute.png");

	LOAD_TEXTURE(L"Slider Tip", L"..\\Resources\\Texture\\UI\\Slider\\Slider Tip.png");
	LOAD_TEXTURE(L"Slider Tip Mute", L"..\\Resources\\Texture\\UI\\Slider\\Slider Tip_mute.png");

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
	LOAD_TEXTURE(L"Frame1", L"..\\Resources\\Texture\\UI\\Frame\\Frame1.png");
	LOAD_TEXTURE(L"Frame1_selected", L"..\\Resources\\Texture\\UI\\Frame\\Frame1_selected.png");
	LOAD_TEXTURE(L"Pop Up Frame", L"..\\Resources\\Texture\\UI\\Frame\\Pop Up Frame.png");

	LOAD_TEXTURE(L"Button2", L"..\\Resources\\Texture\\UI\\Button\\Button2.png");
	LOAD_TEXTURE(L"Button2_selected", L"..\\Resources\\Texture\\UI\\Button\\Button2_selected.png");

	LOAD_TEXTURE(L"Close", L"..\\Resources\\Texture\\UI\\Button\\Close.png");
	LOAD_TEXTURE(L"Close Selected", L"..\\Resources\\Texture\\UI\\Button\\Close_selected.png");

	LOAD_TEXTURE(L"Knight", L"..\\Resources\\Texture\\UI\\Font\\Knight.png");
	LOAD_TEXTURE(L"Mage", L"..\\Resources\\Texture\\UI\\Font\\Mage.png");
	LOAD_TEXTURE(L"Priest", L"..\\Resources\\Texture\\UI\\Font\\Priest.png");
	LOAD_TEXTURE(L"Knight2", L"..\\Resources\\Texture\\UI\\Font\\Knight2.png");
	LOAD_TEXTURE(L"Mage2", L"..\\Resources\\Texture\\UI\\Font\\Mage2.png");
	LOAD_TEXTURE(L"Priest2", L"..\\Resources\\Texture\\UI\\Font\\Priest2.png");
	LOAD_TEXTURE(L"Ready", L"..\\Resources\\Texture\\UI\\Font\\Ready.png");

	LOAD_TEXTURE(L"Nana", L"..\\Resources\\Texture\\UI\\Character Select\\Nana.png");
	LOAD_TEXTURE(L"Mistic", L"..\\Resources\\Texture\\UI\\Character Select\\Mistic.png");
	LOAD_TEXTURE(L"Carmel", L"..\\Resources\\Texture\\UI\\Character Select\\Carmel.png");

	return S_OK;
}
#pragma endregion
