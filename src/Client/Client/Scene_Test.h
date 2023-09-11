#pragma once

#include "Client_Defines.h"
#include <Scene.h>
#include <MonoBehaviour.h>
#include <unordered_set>

class Texture;
class InfoUI_Script;
class Fade_Script;
class PortalUI_Script;
class Cinematic_Script;
class Magic_Artifact_Script;

struct EffectInfo
{
	int32_t index;
	Vec3 pos;
	Vec3 scale;
	float speed;
	float alpha = 1.f;
};

enum class MAP_TYPE
{
	StartRoom,
	FirstBoss,
	Cave,
	SecondRoom_Bridge_SecondBoss,
	ThirdRoom_RockRolling,
	LastBoss_TreasureRoom,
	END
};

class Scene_Test final : public CScene
{
private:
	enum SLIDER_TYPE
	{
		BGM,
		SE,

		MAX
	};

	struct UITransform
	{
		Vec2 pos;
		Vec2 scale;
	};

public:
	explicit Scene_Test();
	virtual ~Scene_Test() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

private:
	void CreateLayer(void);
	void CreateComputeShader(void);
	void CreateMainCamera(std::shared_ptr<CScene> pScene);
	void CreateUICamera(std::shared_ptr<CScene> pScene);
	void CreateSkyBox(std::shared_ptr<CScene> pScene);
	void CreateUI(std::shared_ptr<CScene> pScene, server::FBX_TYPE player);
	void CreateMRTUI(std::shared_ptr<CScene> pScene);
	void CreatePortalUI(std::shared_ptr<CScene> pScene);
	void CreateLights(std::shared_ptr<CScene> pScene);
	void CreateMap(std::shared_ptr<CScene> pScene);
	void CreatePlayer(std::shared_ptr<CScene> pScene, server::FBX_TYPE player);
	void CreateSphere(std::shared_ptr<CScene> pScene);
	void CreateSkill(std::shared_ptr<CScene> pScene);
	void CreateSkill(const std::wstring& colorName, const Vec3& worldPos, const Vec3& localScale, float alpha, float scaleSpeed);
	void CreateBillBoard(std::shared_ptr<CScene> pScene);
	void CreateEffect(std::shared_ptr<CScene> pScene);
	void CreateMagicArtifactEffect(std::shared_ptr<CScene> pScene);

private:
	std::shared_ptr<CGameObject> CreateBillBoardBase(std::vector<shared_ptr<Texture>> textures, float fPassingTime);
	std::shared_ptr<CGameObject> CreateEffectBase(std::vector<shared_ptr<Texture>> textures, float fPassingTime);
	std::vector<std::shared_ptr<CGameObject>> CreateSkillBase(const std::wstring& skillName, const std::wstring& fbxPath);
	std::shared_ptr<CGameObject> CreateArtifactBase(std::vector<shared_ptr<Texture>> textures);
	std::vector<std::shared_ptr<CGameObject>> CreateMapObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> CreateAnimatedObject(ObjectDesc& objectDesc);
	void CreateFade(std::shared_ptr<CScene> pScene);

private:
	void PushMapData(MAP_TYPE eType, std::vector<std::shared_ptr<CGameObject>> objects);

private:
	void CreatePlayerUI(server::FBX_TYPE character);
	void CreatePlayerImage(server::FBX_TYPE character);
	void CreatePlayerImage(server::FBX_TYPE character, UITransform& hpTransform);
	void CreatePlayerName(const std::wstring& name);
	void CreatePlayerHPBar(float yPos, UITransform& hpTransform);
	float CreatePlayerMPBar();

	void CreatePartyPlayerUI(int32_t id, server::FBX_TYPE character, const std::wstring& name);
	void CreatePartyPlayerImage(server::FBX_TYPE character, UITransform trans);
	void CreatePartyPlayerName(const std::wstring& name);
	void CreatePartyPlayerHPBar(float yPos, UITransform trans);
	float CreatePartyPlayerMPBar(UITransform trans);

	void CreateBossUI(server::FBX_TYPE boss, int32_t hp);
	void CreateBossHPBar(UITransform& trans, server::FBX_TYPE boss, int32_t hp);
	void CreateBossClassIcon(UITransform& trans, server::FBX_TYPE boss);
	void CreateBossWarning();

	void CreateOneTimeDialogue();

	void CreatePopUp();
	void CreateBlur();
	void CreateCloseButton();
	void CreateSettingFrame();
	void CreateBGMButton();
	void CreateBGMSlider();
	void CreateSEButton();
	void CreateSESlider();

private:
	void ChangePopUpVisibility();
	void ChangeVolume();
	void ChangeMuteTexture();
	void ChangeBossUIVisibility();

private:
	void SendKeyInput();

	std::vector<std::shared_ptr<CGameObject>> AddNetworkToObject(std::vector<std::shared_ptr<CGameObject>> object, server::OBJECT_TYPE objectType, int32_t id = -1);
	void CreateAnimatedRemoteObject(network::CPacket& packet);
	void CreateRemoteObject(network::CPacket& packet);
	void RemoveObject(network::CPacket& packet);
	void RemoveNonAnimatedObject(server::OBJECT_TYPE type, int32_t id);
	void PlayEffect(network::CPacket& packet);
	void ChangeSound(network::CPacket& packet);
	void TriggerBehaviour(network::CPacket& packet);
	void TriggerInteractionCount(network::CPacket& packet);
	void PlayCutScene(network::CPacket& packet);
	void ChangeMonsterHP(network::CPacket& packet);

	void ClassifyObject(server::FBX_TYPE type, ObjectDesc& objectDesc, int32_t stateIndex = -1);
	void AddObjectToScene(server::OBJECT_TYPE type, std::vector<std::shared_ptr<CGameObject>>& gameObjects);
	void AddEffectTextures();

private:
	void CheckMapMove(void);
	void MoveMap(MAP_TYPE eType);

private:
	void Init(std::shared_ptr<Scene_Test> pScene, server::FBX_TYPE eType);

public:
	static std::shared_ptr<CScene> Create(server::FBX_TYPE eType);

private:
	std::unordered_set<int32_t> m_overlappedObjects;

private:
	std::vector<int32_t> m_effects;
	std::unordered_map<server::EFFECT_TYPE, EffectInfo> m_billboardInfo;

private:
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_1;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_2;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_3;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_4;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_5;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_6;

	MAP_TYPE m_eCurrentMapType;
	MAP_TYPE m_eNextMapType;

private:
	uint32_t m_fireballEffectStartIndex;
	uint32_t m_fireballEffectCurrentIndex;

	uint32_t m_explodeEffectStartIndex;
	uint32_t m_explodeEffectCurrentIndex;

	uint32_t m_iceExplodeEffectStartIndex;
	uint32_t m_iceExplodeEffectCurrentIndex;

	uint32_t m_spiralEffectStartIndex;
	uint32_t m_spiralEffectCurrentIndex;

	uint32_t m_hitEffectStartIndex;
	uint32_t m_hitEffectCurrentIndex;

	uint32_t m_electricDarkGrayEffectStartIndex;
	uint32_t m_electricDarkGrayEffectCurrentIndex;

	uint32_t m_wpDarkBlueEffectStartIndex;
	uint32_t m_wpDarkBlueEffectCurrentIndex;

	std::vector<std::shared_ptr<class Bomb_Script>> m_skillObject;

private:
	std::shared_ptr<InfoUI_Script> m_InfoUIScript;
	std::shared_ptr<Fade_Script> m_fadeScript;
	std::shared_ptr<PortalUI_Script> m_portalUIScript;
	std::shared_ptr<Cinematic_Script> m_cinematicScript;

	std::vector<std::shared_ptr<Magic_Artifact_Script>> m_artifactMagicScript;
	std::unordered_map<std::string, std::shared_ptr<class OneTimeDialogue_Script>> m_oneTimeDialogueScript;

	std::vector<std::shared_ptr<CGameObject>> m_popUp;
	std::unordered_map<int32_t, UITransform> m_partyUITransform;

	std::shared_ptr<class CloseButton_Script> m_closeButton;
	std::vector<std::shared_ptr<class SliderTip_Script>> m_sliderTip;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSlider;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSliderLeftTip;
	std::vector<std::shared_ptr<class MuteButton_Script>> m_muteButton;

	std::vector<std::shared_ptr<CGameObject>> m_weeperUIObjets;
	std::vector<std::shared_ptr<CGameObject>> m_golemUIObjets;
	std::shared_ptr<class BossHP_Script> m_weeperHPScript;
	std::shared_ptr<class BossHP_Script> m_golemHPScript;
	std::vector<std::shared_ptr<class BossWarning_Script>> m_bossWarningScript;

	bool m_openSetting;

private:
	std::shared_ptr<bool> m_recvFadeIn;
	std::shared_ptr<bool> m_recvFadeOut;
	std::shared_ptr<bool> m_recvExplosionSkill;
};