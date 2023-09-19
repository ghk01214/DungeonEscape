#include "pch.h"
#include "Loader.h"
#include "Resources.h"
#include "MeshData.h"
#include "Engine.h"

std::shared_ptr<CLoader> CLoader::Create(SCENE eScene)
{
	shared_ptr<CLoader> pInstance = make_shared<CLoader>();

	if (FAILED(pInstance->Loading(eScene)))
	{
		MSG_BOX(TEXT("Failed to Created CLoader"));
	}

	return pInstance;
}

uint32_t APIENTRY ThreadEntryFunc(CLoader* pArg)
{
	//std::shared_ptr<CLoader> pLoader = static_cast<std::shared_ptr<CLoader>>(pArg);
	//std::shared_ptr<CLoader> pLoader = pArg;

	CLoader* pLoader = reinterpret_cast<CLoader*>(pArg);

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

CLoader::CLoader() :
	m_eScene{ SCENE_END },
	m_isFinished{ false },
	m_hThread{ 0 }
{
}

HRESULT CLoader::Loading(SCENE eScene)
{
	m_eScene = eScene;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	InitializeCriticalSection(&m_CriticalSection);

	std::thread loaderThread = std::thread{ ThreadEntryFunc, this };

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
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Carmel\\Carmel.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\Weeper\\Weeper1.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\MoltenGolem\\Blue Golem.fbx");
	//GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Character\\StylizedScorpion\\Black Scorpion.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Sphere.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Ice Ball.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Skill\\Poison Ball.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Models.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Models2.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Pillar Bridge.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Rolling Rock.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Scatter Rock.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone1.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone2.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Stone3.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Skill\\Sphere\\Orange.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Skill\\Sphere\\Red.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Skill\\Sphere\\White.fbx");
	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Skill\\Sphere\\Yellow.fbx");

	GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Models\\Sphere2.fbx");

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_UI()
{
	LOAD_TEXTURE(L"Nana_In Game", L"..\\Resources\\Texture\\UI\\In Game\\Nana.png");
	LOAD_TEXTURE(L"Mistic_In Game", L"..\\Resources\\Texture\\UI\\In Game\\Mistic.png");

	LOAD_TEXTURE(L"Weeper Class", L"..\\Resources\\Texture\\UI\\Icon\\Weeper Class.png");
	LOAD_TEXTURE(L"Golem Class", L"..\\Resources\\Texture\\UI\\Icon\\Golem Class.png");

	LOAD_TEXTURE(L"Pillar Hint", L"..\\Resources\\Texture\\UI\\Font\\Pillar Hint.png");
	LOAD_TEXTURE(L"Pillar Hint2", L"..\\Resources\\Texture\\UI\\Font\\Pillar Hint2.png");

	LOAD_TEXTURE(L"Ending", L"..\\Resources\\Texture\\UI\\Font\\Ending.png");

	LOAD_TEXTURE(L"Weeper Hint", L"..\\Resources\\Texture\\UI\\Font\\Weeper Hint.png");
	LOAD_TEXTURE(L"Weeper Cast4 Hint", L"..\\Resources\\Texture\\UI\\Font\\Weeper Cast4 Hint.png");
	LOAD_TEXTURE(L"Golem Hint", L"..\\Resources\\Texture\\UI\\Font\\Golem Hint.png");
	LOAD_TEXTURE(L"Boss Warning", L"..\\Resources\\Texture\\UI\\Font\\Boss Warning.png");
	LOAD_TEXTURE(L"Red Blur", L"..\\Resources\\Texture\\UI\\Blur\\Red.png");

	LOAD_TEXTURE(L"Fade Blur", L"..\\Resources\\Texture\\UI\\Blur\\White.png");
	LOAD_TEXTURE(L"Move Map", L"..\\Resources\\Texture\\UI\\Font\\Move Map.png");

	LOAD_TEXTURE(L"Player Slider Frame(C)", L"..\\Resources\\Texture\\UI\\Slider\\Slider02_Frame(C).png");
	LOAD_TEXTURE(L"HP(C)", L"..\\Resources\\Texture\\UI\\Slider\\Slider02_Fill_Red(C).png");

#pragma region [POP UP]
	LOAD_TEXTURE(L"Blur", L"..\\Resources\\Texture\\UI\\Blur\\Dimed.png");
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

	LOAD_TEXTURE(L"Weeper Tutorial1", L"..\\Resources\\Texture\\UI\\In Game\\Weeper Tutorial1.png");
	LOAD_TEXTURE(L"Weeper Tutorial2", L"..\\Resources\\Texture\\UI\\In Game\\Weeper Tutorial2.png");
	LOAD_TEXTURE(L"Golem Tutorial1", L"..\\Resources\\Texture\\UI\\In Game\\Golem Tutorial1.png");
	LOAD_TEXTURE(L"Golem Tutorial2", L"..\\Resources\\Texture\\UI\\In Game\\Golem Tutorial2.png");

	LOAD_TEXTURE(L"Prev Button", L"..\\Resources\\Texture\\UI\\Button\\Prev.png");
	LOAD_TEXTURE(L"Prev Button_selected", L"..\\Resources\\Texture\\UI\\Button\\Prev_selected.png");
	LOAD_TEXTURE(L"Next Button", L"..\\Resources\\Texture\\UI\\Button\\Next.png");
	LOAD_TEXTURE(L"Next Button_selected", L"..\\Resources\\Texture\\UI\\Button\\Next_selected.png");

	LOAD_TEXTURE(L"Progress Open", L"..\\Resources\\Texture\\UI\\Button\\Info.png");
	LOAD_TEXTURE(L"Progress Open_selected", L"..\\Resources\\Texture\\UI\\Button\\Info_selected.png");
	LOAD_TEXTURE(L"Question", L"..\\Resources\\Texture\\UI\\Button\\Question.png");
	LOAD_TEXTURE(L"Question_selected", L"..\\Resources\\Texture\\UI\\Button\\Question_selected.png");
	LOAD_TEXTURE(L"Phase1", L"..\\Resources\\Texture\\UI\\In Game\\Phase1.png");
	LOAD_TEXTURE(L"Phase2", L"..\\Resources\\Texture\\UI\\In Game\\Phase2.png");
	LOAD_TEXTURE(L"Phase3", L"..\\Resources\\Texture\\UI\\In Game\\Phase3.png");
	LOAD_TEXTURE(L"Phase4", L"..\\Resources\\Texture\\UI\\In Game\\Phase4.png");
	LOAD_TEXTURE(L"Phase5", L"..\\Resources\\Texture\\UI\\In Game\\Phase5.png");
	LOAD_TEXTURE(L"Phase6", L"..\\Resources\\Texture\\UI\\In Game\\Phase6.png");
	LOAD_TEXTURE(L"Phase7", L"..\\Resources\\Texture\\UI\\In Game\\Phase7.png");

	LOAD_TEXTURE(L"Nana Progress", L"..\\Resources\\Texture\\UI\\In Game\\Nana Progress.png");
	LOAD_TEXTURE(L"Mistic Progress", L"..\\Resources\\Texture\\UI\\In Game\\Mistic Progress.png");

	LOAD_TEXTURE(L"Nana Gimmick Tutorial", L"..\\Resources\\Texture\\UI\\In Game\\Nana Gimmick Tutorial.png");
	LOAD_TEXTURE(L"Mistic Gimmick Tutorial", L"..\\Resources\\Texture\\UI\\In Game\\Mistic Gimmick Tutorial.png");
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_GamePlayLevel_Effect()
{
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Artifact_Protection", L"..\\Resources\\Texture\\Effect\\Artifact Protection\\tile.png", 16);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Bright_Flare_Dark_Gray", L"..\\Resources\\Texture\\Effect\\Bright Flare\\dark gray\\tile.png", 32);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Circle_Flame", L"..\\Resources\\Texture\\Effect\\Circle Flame\\yellow\\tile.png", 24);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Circle_Wave", L"..\\Resources\\Texture\\Effect\\Circle Wave\\tile.png", 6);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Explode", L"..\\Resources\\Texture\\Effect\\Explode\\tile.png", 20);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Ice_Explode", L"..\\Resources\\Texture\\Effect\\Ice Explode\\tile.png", 15);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Flash_In_Dark", L"..\\Resources\\Texture\\Effect\\Flash In\\dark\\tile.png", 34);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Fire\\Fire.png", 64);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Hit", L"..\\Resources\\Texture\\Effect\\Hit\\tile.png", 4);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_In_Dispersal", L"..\\Resources\\Texture\\Effect\\In Dispersal\\tile.png", 33);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Implode_Orange_Brown", L"..\\Resources\\Texture\\Effect\\Implode\\orange brown\\tile.png", 29);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Paralys", L"..\\Resources\\Texture\\Effect\\Paralys\\tile.png", 21);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Shield_Electric_Dark_Gray", L"..\\Resources\\Texture\\Effect\\Shield Electric\\dark gray\\tile.png", 30);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Slash_Special", L"..\\Resources\\Texture\\Effect\\Slash Special\\tile.png", 18);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Fire_Sword", L"..\\Resources\\Texture\\Effect\\Fire Sword\\tile.png", 14);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Ice_Sword", L"..\\Resources\\Texture\\Effect\\Ice Sword\\tile.png", 13);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Lightning_Sword", L"..\\Resources\\Texture\\Effect\\Lightning Sword\\Upside Down\\tile.png", 14);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Star_Sword", L"..\\Resources\\Texture\\Effect\\Star Sword\\tile.png", 24);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Lune_Sword", L"..\\Resources\\Texture\\Effect\\Lune Sword\\tile.png", 28);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Cross_Sword", L"..\\Resources\\Texture\\Effect\\Cross Sword\\tile.png", 19);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Spark_Particle", L"..\\Resources\\Texture\\Effect\\Spark Particle\\tile.png", 25);

	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Spiral", L"..\\Resources\\Texture\\Effect\\Spiral\\tile.png", 18);
	GET_SINGLE(Resources)->LoadEffectTextures(L"Effect_Smokey_Swirl", L"..\\Resources\\Texture\\Effect\\smokey swirl\\tile.png", 48);

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
	LOAD_TEXTURE(L"Lobby_InGame", L"..\\Resources\\Texture\\UI\\Start\\Background.PNG");

	LOAD_TEXTURE(L"Title", L"..\\Resources\\Texture\\UI\\Font\\Title.png");
	LOAD_TEXTURE(L"Log In", L"..\\Resources\\Texture\\UI\\Font\\Log In.png");

	LOAD_TEXTURE(L"Button", L"..\\Resources\\Texture\\UI\\Button\\Button.png");
	LOAD_TEXTURE(L"Button Selected", L"..\\Resources\\Texture\\UI\\Button\\Button_selected.png");
	LOAD_TEXTURE(L"Setting", L"..\\Resources\\Texture\\UI\\Button\\Setting.png");

#pragma region [POP UP]
#pragma region [SETTING]
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
#pragma endregion
#pragma region [LOG_IN]
	LOAD_TEXTURE(L"Log In Frame", L"..\\Resources\\Texture\\UI\\Frame\\Log In Frame.png");

	LOAD_TEXTURE(L"Input Field", L"..\\Resources\\Texture\\UI\\Frame\\InputFrame.png");
	LOAD_TEXTURE(L"Input Field_selected", L"..\\Resources\\Texture\\UI\\Frame\\InputFrame_selected.png");
#pragma endregion
#pragma endregion

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

	LOAD_TEXTURE(L"Button2", L"..\\Resources\\Texture\\UI\\Button\\Button2.png");
	LOAD_TEXTURE(L"Button2_selected", L"..\\Resources\\Texture\\UI\\Button\\Button2_selected.png");

	LOAD_TEXTURE(L"Knight", L"..\\Resources\\Texture\\UI\\Font\\Knight.png");
	LOAD_TEXTURE(L"Mage", L"..\\Resources\\Texture\\UI\\Font\\Mage.png");
	LOAD_TEXTURE(L"Ready", L"..\\Resources\\Texture\\UI\\Font\\Ready.png");

	LOAD_TEXTURE(L"Nana", L"..\\Resources\\Texture\\UI\\Character Select\\Nana.png");
	LOAD_TEXTURE(L"Mistic", L"..\\Resources\\Texture\\UI\\Character Select\\Mistic.png");

#pragma region [POP UP]
	LOAD_TEXTURE(L"Pop Up Frame", L"..\\Resources\\Texture\\UI\\Frame\\Pop Up Frame.png");

	LOAD_TEXTURE(L"Close", L"..\\Resources\\Texture\\UI\\Button\\Close.png");
	LOAD_TEXTURE(L"Close Selected", L"..\\Resources\\Texture\\UI\\Button\\Close_selected.png");

	LOAD_TEXTURE(L"Knight2", L"..\\Resources\\Texture\\UI\\Font\\Knight2.png");
	LOAD_TEXTURE(L"Mage2", L"..\\Resources\\Texture\\UI\\Font\\Mage2.png");
#pragma endregion

	return S_OK;
}
#pragma endregion
